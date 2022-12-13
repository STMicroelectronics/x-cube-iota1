/**
  ******************************************************************************
  * @file   l2sec_example.c
  * @author SRA/Central LAB -- LINKS
  * @brief  Implement L2SEC protocol for encrypted messages over IOTA network.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* uncomment for verbose output for debugging for L2 wrap/unwrap */
//#define IOTA_L2_MSG_DEBUG

/* uncomment for verbose output for debugging for L1 helper functions */
//#define IOTA_L1_MSG_DEBUG

/* uncomment for verbose output for debugging for example core functions */
//#define IOTA_L2SEC_MSG_EXAMPLE_DEBUG

//
// ========================== IOTA L2Sec wrap/unrap  =========================
//

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "sodium.h"
#include "main.h"

#include "crypto/iota_crypto.h"
#include "core/seed.h"
#include "core/address.h"
#include "core/utils/byte_buffer.h"

/* to use function to get the JSON-formatted sensor data */
#include "sensors.h"
#include <time.h>
#include "utarray.h"

//#include "client/network/http.h"
#include "client/api/v1/send_message.h"
#include "client/api/v1/get_message.h"
#include "client/api/v1/find_message.h"

#ifdef L2SEC_WITH_STSAFE
#include "stsafe.h"
#include "stsafea_types.h"
#endif /* L2SEC_WITH_STSAFE */

#include "iota_conf.h"



/* Global Vars ---------------------------------------------------------------*/
extern RNG_HandleTypeDef hrng;


/* Constants -----------------------------------------------------------------*/
#define MSG_DATA "STMicroelectronics - L2Sec Demo:"
#define STR_BUFFER_SIZE (300)

/* RAW SIZEs */
#define MAX_DATA_HEX_LENGTH (900 + 1)
#define MAX_INDEX_HEX_LENGTH (64 + 1)

#define FIRST_ITERATION 0

#define IOTA_L2SEC_PSK "6E93DFA4DEAAE33E0135DFF4A6BB678FA7FFDC10869ADC6E6D38DDCBC90C39FF"

#define MAX_L1_RETRY    5

/* constants for NOPOLL/POLL mode when reading messages from the Tangle */ 
#define READ_MODE_NOPOLL 0
#define READ_MODE_POLL 1

/* number of messages to send to the channel as a stream */
/* 0 = (almost) infinite loop: 4Giga iterations */
#define N_MESSAGES 3

/* Private function declarations ---------------------------------------------*/
#ifndef L2SEC_WITH_STSAFE
static void randomize_seed(uint8_t *arr, uint16_t arr_len);
#endif

static int iota_l2sec_wrap (int iter,
                            byte_t *seed,
                            byte_buf_t* data,
                            byte_buf_t* buffer,
                            byte_t *next_seed,
                            byte_t* index);

static int iota_l2sec_unwrap (byte_buf_t* buffer,
                             byte_t *index,
                             byte_buf_t* recv_data,
                             byte_t *recv_next_index);

static int send_l1_message(byte_buf_t* app_data,
                           byte_t* index,
                           char* msg_id);

static int get_msg_by_id (char *msg_id, char *data, char *index);
static int get_l1_message_by_id (char *msg_id,
                                 byte_buf_t* app_data,
                                 byte_t* index);
static int get_app_data (byte_buf_t* data_to_send, int n_iter);
static void example_receive_l2sec_protected_msg_by_id(char *msg_id,
                                                      byte_t *index,
                                                      byte_t *next_index);
static int example_receive_l2sec_protected_msg_by_index(byte_t *index,
                                                        byte_t *next_index,
                                                        int poll);
static void example_send_l2sec_protected_stream(int n_messages, byte_t *first_index);
static void example_receive_l2sec_protected_stream(byte_t* index, int poll);

/* Private function definitions ----------------------------------------------*/

#ifndef L2SEC_WITH_STSAFE
static void randomize_seed(uint8_t *arr, uint16_t arr_len)
{
	uint16_t i = 0;
	uint32_t myrnd = 0;

	for(i = 0; i < arr_len; i+=4) {
		HAL_RNG_GenerateRandomNumber(&hrng, &myrnd);

		arr[i+0] = (myrnd & 0x000000ff);
		arr[i+1] = (myrnd & 0x0000ff00) >> 8;
		arr[i+2] = (myrnd & 0x00ff0000) >> 16;
		arr[i+3] = (myrnd & 0xff000000) >> 24;
	}
}
#endif

/*
 * =============================================================================
 *
 * STSAFE code
 *
 * =============================================================================
 */

/*
 * NOTE: the following STSAFE code is an excerpt from the authentication example
 *       from the STM X-CUBE-SAFEA1 function package
 */

//
// =========================== L2 wrap/unwarp functions =========================
//

/**
 * @brief     iota_l2sec_wrap
 *            This function wraps an app data message into an L2Sec payload
 * @note      This function encodes the app data length, the data, the
 *            public key required for the signature verification during
 *            the decode phase (within iota_l2sec_unwrap function), the
 *            next_index, the signature and the auth_signature; then
 *            encrypts the encoded payload and returns the encrypted
 *            payload (nonce, cypertext) to the caller for delivery in
 *            an indexation message.
 *
 * IOTA L2 sec message encoding:
 *
 * uint16_t l2_payload_length // app data length [network order byte]
 * byte_t l2_payload[]        // app data
 * byte_t pubkey[32]          // public key to verify the signature 
 * byte_t next_iota_index[32] // index to retrieve the next message
 * byte_t signature[64]       // signature over the preceding fields
 * byte_t auth_signature[64]  // Authentication Signature
 *
 * 
 * @param[in] iter   : indicates if this is the first iteration (iter = 0)
 * @param[in] seed   : empty byte array allocated by the caller (first iterat.)
 *                     and seed passed by the caller (from second iteration on)
 * @param[in] data   : buffer containing binary data to be sent 
 * @param[out] buffer : buffer allocated by caller to store temporarily the
 *                      encoded data and finally the encrypted data to be 
 *                      returned to the caller 
 * @param[out] next_seed  : byte array allocated by the caller; next_seed 
 *                          generated randomly inside the function and passed
 *                          to the caller that will copy to seed param at the
 *                          next iteration 
 * @param[out] index  : byte array allocated by the caller; index generated 
 *                         in the function from the pubkey which is in turn 
 *                         generated from the seed; it is returned to the caller 
 *                         to  next_seed generated randomly inside the function
 * 
 * @retval    0 on success, an error code otherwise.
 */
static int iota_l2sec_wrap (int iter,
                            byte_t *seed,
                            byte_buf_t* data,
                            byte_buf_t* buffer,
                            byte_t *next_seed,
                            byte_t* index)
{

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nENTERED function: %s\n", __func__);
#endif /* IOTA_L2_MSG_DEBUG */

  /* generate keypair and iota_index */
  iota_keypair_t keypair;
  memset(&keypair, 0, sizeof(keypair));

  if (iter == FIRST_ITERATION) { /* first L2 message of the chain: use a fresh seed */

#ifdef L2SEC_WITH_STSAFE
	//dump_hex_str(seed, IOTA_SEED_BYTES);
    printf("STSAFE: Random - Generate ");
    stsafe_GenRandom(seed, IOTA_SEED_BYTES);
    printf("-> OK\n");
    //dump_hex_str(seed, IOTA_SEED_BYTES);
#else /* L2SEC_WITH_STSAFE */
    printf ("NO STSAFE: first secret generated in software\n\n");
    //random_seed(seed);
    randomize_seed(seed, IOTA_SEED_BYTES);
#endif /* L2SEC_WITH_STSAFE */

#ifdef IOTA_L2_MSG_DEBUG
    printf("First L2 message of the chain: use a newly generated seed\n\n");
#endif /* IOTA_L2_MSG_DEBUG */
  }
  else { /* subsequent L2 message of the chain: use the passed seed */ 
#ifdef IOTA_L2_MSG_DEBUG
    printf("Subsequent L2 message of the chain: use seed passed as param\n\n");
#endif /* IOTA_L2_MSG_DEBUG */
  }

  iota_crypto_keypair(seed, &keypair);
  address_from_ed25519_pub(keypair.pub, index);

#ifdef IOTA_L2_MSG_DEBUG
  printf("Seed: ");
  dump_hex_str(seed, IOTA_SEED_BYTES);
  printf("Index: ");
  dump_hex_str(index, ED25519_ADDRESS_BYTES);
#endif /* IOTA_L2_MSG_DEBUG */

  /* generate next_keypair and next_iota_index */ 
  byte_t next_index[ED25519_ADDRESS_BYTES];
  memset(next_index, 0, sizeof(next_index));
  iota_keypair_t next_keypair;
  memset(&next_keypair, 0, sizeof(next_keypair));

#ifdef L2SEC_WITH_STSAFE
  printf("STSAFE: Random - Generate ");
  stsafe_GenRandom(next_seed, IOTA_SEED_BYTES);
  printf("-> OK\n");
#else
//  random_seed(next_seed);
  randomize_seed(next_seed, IOTA_SEED_BYTES);
#endif /* L2SEC_WITH_STSAFE */

  iota_crypto_keypair(next_seed, &next_keypair);
  address_from_ed25519_pub(next_keypair.pub, next_index);

#ifdef IOTA_L2_MSG_DEBUG
  printf("Next seed: ");
  dump_hex_str(next_seed, IOTA_SEED_BYTES);
  printf("Next index: ");
  dump_hex_str(next_index, ED25519_ADDRESS_BYTES);
#endif /* IOTA_L2_MSG_DEBUG */
 
  /* append data length */
  uint16_t data_length = data->len;
  byte_buf_append(buffer, (byte_t const*)&data_length, sizeof(data_length));

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nAdded data length to buffer (%u)\n", data_length);
  byte_buf_print(buffer);
#endif /* IOTA_L2_MSG_DEBUG */

  /* append data */
  byte_buf_append(buffer, (byte_t const*)data->data, data_length); 
  //printf ("Buffer: %s\n", buffer->data);
#ifdef IOTA_L2_MSG_DEBUG
  // FIXME: to be updated to support binary data
  printf("\nAdded data to buffer (%s)\n", data->data);
  byte_buf_print(buffer);
#endif /* IOTA_L2_MSG_DEBUG */

  /* append public key */
  byte_buf_append(buffer, keypair.pub, sizeof(keypair.pub)); 
#ifdef IOTA_L2_MSG_DEBUG
  printf("\nAdded public key to buffer (length: %d)\n", sizeof(keypair.pub));
  byte_buf_print(buffer);
#endif /* IOTA_L2_MSG_DEBUG */

  /* append next_index */
  byte_buf_append(buffer, next_index, ED25519_ADDRESS_BYTES); 
#ifdef IOTA_L2_MSG_DEBUG
  printf("\nAdded next index to buffer (length: %d)\n", ED25519_ADDRESS_BYTES);
  byte_buf_print(buffer);
#endif /* IOTA_L2_MSG_DEBUG */

  /*
   * generate signature 
   */
  byte_t buffer_hash[CRYPTO_BLAKE2B_HASH_BYTES];
  memset(buffer_hash, 0, sizeof(buffer_hash));
  byte_t signature[ED_SIGNATURE_BYTES];
  memset(signature, 0, sizeof(signature));

  /* calculate hash */
  if (iota_blake2b_sum(buffer->data, buffer->len, buffer_hash, sizeof(buffer_hash)) != 0) {
    printf("[%s:%d] get buffer hash failed\n", __func__, __LINE__);
    return -1;
  }

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nHash calculated over the send buffer: ");
  dump_hex_str(buffer_hash, CRYPTO_BLAKE2B_HASH_BYTES);
#endif /* IOTA_L2_MSG_DEBUG */

  /* calculate signature */
  if (iota_crypto_sign(keypair.priv, buffer_hash, sizeof(buffer_hash), signature) != 0) {
    printf("[%s:%d] get signature failed\n", __func__, __LINE__);
    return -1;
  }

  /* append signature */
  byte_buf_append(buffer, signature, sizeof(signature)); 
#ifdef IOTA_L2_MSG_DEBUG
  printf("\nAdded signature to buffer (length: %d)\n", sizeof(signature));
  dump_hex_str(signature, sizeof(signature));
  byte_buf_print(buffer);
#endif /* IOTA_L2_MSG_DEBUG */

#ifdef L2SEC_WITH_STSAFE
  /* calculate and append authentication signature */
  printf("STSAFE: Authentication Signature - Sign ");
  uint8_t auth_sign[STSAFEA_SIGN_ECDSA_SHA256_LENGTH] = {0};
  stsafe_AuthSign_sign(buffer->data, buffer->len, auth_sign);
  printf("-> OK\n");
  byte_buf_append(buffer, auth_sign, sizeof(auth_sign));
#endif /* L2SEC_WITH_STSAFE */

  /*
   * encrypt the buffer using sodium's AEAD secretbox API:
   * - Encryption: XSalsa20 stream cipher
   * - Authentication: Poly1305 MAC
   */

  // FIXME: try to define them as byte_t
  /* allocate buffers */
  unsigned char key[crypto_secretbox_KEYBYTES];
  unsigned char nonce[crypto_secretbox_NONCEBYTES];
  unsigned char ciphertext[crypto_secretbox_MACBYTES + buffer->len];

#ifdef L2SEC_WITH_STSAFE
  printf("STSAFE: PSK - Unwrap ");
  stsafe_PSK_unwrap(key);
  printf("-> OK\n");

#else
  /* copy the hardcoded PSK into the key buffer */
  hex_2_bin(IOTA_L2SEC_PSK, strlen(IOTA_L2SEC_PSK), 
            key, crypto_secretbox_KEYBYTES);
#endif /* L2SEC_WITH_STSAFE */

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nDump symmetric key (length %d): ", crypto_secretbox_KEYBYTES);
  dump_hex_str(key, crypto_secretbox_KEYBYTES);
#endif /* IOTA_L2_MSG_DEBUG */


  /* getting the random nonce as required by the secretbox API */
#ifdef L2SEC_WITH_STSAFE
  printf("STSAFE: Random - Generate ");
  stsafe_GenRandom(nonce, sizeof(nonce));
  printf("-> OK\n");
#else
  randomize_seed(nonce, crypto_secretbox_NONCEBYTES);
//  iota_crypto_randombytes(nonce, sizeof (nonce));
#endif /* L2SEC_WITH_STSAFE */

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nDump nonce (length %d): ", sizeof(nonce));
  dump_hex_str(nonce, sizeof(nonce));
#endif /* IOTA_L2_MSG_DEBUG */

  /* encrypt the buffer with AEAD encryption */
  crypto_secretbox_easy(ciphertext, buffer->data, buffer->len, nonce, key);
#ifdef L2SEC_WITH_STSAFE
  printf("Cleaning PSK from RAM... ");
  memset(key, 0, crypto_secretbox_KEYBYTES);   // clean key after use
  printf("-> OK\n");
#endif /* L2SEC_WITH_STSAFE */

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nDump the encrypted stream (length %d): ", sizeof(ciphertext));
  dump_hex_str(ciphertext, sizeof(ciphertext));
#endif /* IOTA_L2_MSG_DEBUG */

  /* enlarge the buffer to store now the encrypted stream which is larger than
   * the plaintext, because of the MAC and of the nonce */
  byte_buf_reserve(buffer, sizeof(ciphertext) + sizeof(nonce));
  buffer->len = sizeof(ciphertext) + sizeof(nonce);

  /* copy the nonce and the encrypted stream to the buffer for sending */
  memcpy(buffer->data, nonce, sizeof(nonce));
  memcpy(buffer->data + sizeof(nonce), ciphertext, sizeof(ciphertext));

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nDump the buffer to send, now containing the nonce and the encrypted stream:\n");
  byte_buf_print(buffer);
#endif /* IOTA_L2_MSG_DEBUG */

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nEXITING function: %s\n\n", __func__);
#endif /* IOTA_L2_MSG_DEBUG */

  return 0;
}


/**
 * @brief     iota_l2sec_unwrap
 *            This function unwraps an appl. data msg from an L2Sec payload
 * @note      This function decrypts and decodes the L2Sec payload extracted
 *            from the indexation message retrieved from the Tangle.
 *
 * IOTA L2 sec message encoding:
 *
 * uint16_t l2_payload_length // app data length [network order byte]
 * byte_t l2_payload[]        // app data
 * byte_t pubkey[32]          // public key to verify the signature 
 * byte_t next_iota_index[32] // index to retrieve the next message
 * byte_t signature[64]       // signature over the preceeding fields
 * byte_t auth_signature[64]  // Authentication Signature
 *
 * 
 * @param[in] buffer : buffer allocated by caller containing the (encrypted) 
 *                     L2Sec payload received from the Tangle
 * @param[in] index  : byte array allocated by the caller; index corresponding
 *                     to the received payload and passed by the caller: it
 *                     is required to verify the channel ownership (it must
 *                     be equal to the hash of the pubkey decoded from the
 *                     L2Sec payload)
 * @param[out] recv_data   : buffer allocated by the caller and containing
 *                           the decoded app data to be returned to the caller
 * @param[out] recv_next_index  : byte array allocated by the caller; 
 *                          the decoded next_index is returned to the caller  
 *                          as the search index for the next indexation message
 *                          in the l2sec chain
 * 
 * @retval    0 on success, an error code otherwise.
 */
static int iota_l2sec_unwrap (byte_buf_t* buffer,
                              byte_t *index,
                              byte_buf_t* recv_data,
                              byte_t *recv_next_index)
{

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nENTERED function: %s\n", __func__);
#endif /* IOTA_L2_MSG_DEBUG */

  /*
   * decrypt the received buffer using sodium's AEAD secretbox API:
   * - Encryption: XSalsa20 stream cipher
   * - Authentication: Poly1305 MAC
   */


  // FIXME: try to define them as byte_t
  /* allocate buffers */
  unsigned char key[crypto_secretbox_KEYBYTES];
  unsigned char nonce[crypto_secretbox_NONCEBYTES];
  unsigned char decrypted[buffer->len - crypto_secretbox_MACBYTES- sizeof(nonce)];

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nDump the received buffer, containing the nonce and the encrypted stream:\n");
  byte_buf_print(buffer);
#endif /* IOTA_L2_MSG_DEBUG */

#ifdef L2SEC_WITH_STSAFE
  printf("STSAFE: PSK - Unwrap ");
  stsafe_PSK_unwrap(key);
  printf("-> OK\n");

#else
  /* copy the hardcoded PSK into the key buffer */
  hex_2_bin(IOTA_L2SEC_PSK, strlen(IOTA_L2SEC_PSK), 
            key, crypto_secretbox_KEYBYTES);
#endif /* L2SEC_WITH_STSAFE */

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nDump symmetric key (length %d): ", crypto_secretbox_KEYBYTES);
  dump_hex_str(key, crypto_secretbox_KEYBYTES);
#endif /* IOTA_L2_MSG_DEBUG */


  /* getting the nonce received in the buffer */
  memcpy(nonce, buffer->data, sizeof(nonce));
#ifdef IOTA_L2_MSG_DEBUG
  printf("\nDump nonce (length %d): ", sizeof(nonce));
  dump_hex_str(nonce, sizeof(nonce));
#endif /* IOTA_L2_MSG_DEBUG */

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nDump the received encrypted stream (length %d): ", 
         buffer->len - sizeof(nonce));
  dump_hex_str(buffer->data + sizeof(nonce), buffer->len - sizeof(nonce));
#endif /* IOTA_L2_MSG_DEBUG */

  /* decrypt the AEAD encrypted stream */
  if (crypto_secretbox_open_easy(decrypted,
                                 buffer->data + sizeof(nonce),
                                 buffer->len - sizeof(nonce),
                                 nonce,
                                 key) != 0) {
    printf("[%s:%d] MAC verification failed during decryption !\n", __func__, __LINE__);
    return -1;
  }

#ifdef L2SEC_WITH_STSAFE
  printf("Cleaning PSK from RAM... ");
  memset(key, 0, crypto_secretbox_KEYBYTES);   // clean key after use
  printf("-> OK\n");
#endif /* L2SEC_WITH_STSAFE */

  /*
   * parse buffer
   */
#ifdef IOTA_L2_MSG_DEBUG
  printf("\nDump the decrypted L2Sec payload (length %d): ", sizeof(decrypted));
  dump_hex_str(decrypted, sizeof(decrypted));
#endif /* IOTA_L2_MSG_DEBUG */

  /* copy the decrypted L2Sec payload to the buffer for the decoding */
  memcpy(buffer->data, decrypted, sizeof(decrypted));
  buffer->len = sizeof(decrypted);

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nReceived buffer with L2-wrapped application data:\n");
  byte_buf_print(buffer);
#endif /* IOTA_L2_MSG_DEBUG */

  /* parse data length */
  size_t i = 0;

  uint16_t recv_data_len;
  memcpy((byte_t *) &recv_data_len, buffer->data, sizeof(recv_data_len));
//  uint16_t recv_data_length = NET_HTONS (recv_data_length_nob); /* convert 2 net order byte */
  uint16_t recv_data_length = recv_data_len;
#ifdef IOTA_L2_MSG_DEBUG
  printf("\nReceived data with length: %d\n", recv_data_length);
#endif /* IOTA_L2_MSG_DEBUG */

  /* move the buffer index ahead */
  i += sizeof(recv_data_len);

  /* parse data */
  byte_buf_append(recv_data, buffer->data + i, recv_data_length); 
#ifdef IOTA_L2_MSG_DEBUG
  // FIXME: to be updated to support binary data
  printf("\nReceived data: %s\n", recv_data->data);
#endif /* IOTA_L2_MSG_DEBUG */

  /* move the buffer index ahead */
  i += recv_data_length;

  /* parse public key */
  byte_t recv_pubkey[ED_PUBLIC_KEY_BYTES];
  memcpy(recv_pubkey, buffer->data + i, ED_PUBLIC_KEY_BYTES);
#ifdef IOTA_L2_MSG_DEBUG
  printf("Received public key: ");
  dump_hex_str(recv_pubkey, ED_PUBLIC_KEY_BYTES);
#endif /* IOTA_L2_MSG_DEBUG */

  /* move the buffer index ahead */
  i += ED_PUBLIC_KEY_BYTES;

  /* parse next_index */
  //byte_t recv_next_index[ED25519_ADDRESS_BYTES];
  memcpy(recv_next_index, buffer->data + i, ED25519_ADDRESS_BYTES);
#ifdef IOTA_L2_MSG_DEBUG
  printf("Received next_index: ");
  dump_hex_str(recv_next_index, ED25519_ADDRESS_BYTES);
#endif /* IOTA_L2_MSG_DEBUG */

  /* move the buffer index ahead */
  i += ED25519_ADDRESS_BYTES;

  /* parse signature */
  byte_t recv_signature[ED_SIGNATURE_BYTES];
  memcpy(recv_signature, buffer->data + i, ED_SIGNATURE_BYTES);
#ifdef IOTA_L2_MSG_DEBUG
  printf("Received signature: ");
  dump_hex_str(recv_signature, ED_SIGNATURE_BYTES);
#endif /* IOTA_L2_MSG_DEBUG */

  /*
   * verify signature 
   */
  byte_t recv_buffer_hash[CRYPTO_BLAKE2B_HASH_BYTES];
  memset(recv_buffer_hash, 0, sizeof(recv_buffer_hash));

  /* calculate hash over the received buffer (exluding the signature) */
  if (iota_blake2b_sum(buffer->data,
                       i,
                       recv_buffer_hash,
                       sizeof(recv_buffer_hash)) != 0) {
    printf("[%s:%d] get buffer hash failed\n", __func__, __LINE__);
    return -1;
  }
#ifdef IOTA_L2_MSG_DEBUG
  printf("Hash calculated over the received buffer: ");
  dump_hex_str(recv_buffer_hash, CRYPTO_BLAKE2B_HASH_BYTES);
#endif /* IOTA_L2_MSG_DEBUG */

  /* signature verification */
  if (crypto_sign_ed25519_verify_detached(recv_signature,
                                          recv_buffer_hash,
                                          CRYPTO_BLAKE2B_HASH_BYTES,
                                          recv_pubkey) != 0) {
    printf("[%s:%d] signature verification failed\n", __func__, __LINE__);
    return -1;
  }
#ifdef IOTA_L2_MSG_DEBUG
  else {
    printf("Signature successfully verified !\n");
  }
#endif /* IOTA_L2_MSG_DEBUG */

  /*
   * channel ownership verification
   */
  byte_t pubkey_buffer_hash[CRYPTO_BLAKE2B_HASH_BYTES];
  memset(pubkey_buffer_hash, 0, sizeof(pubkey_buffer_hash));

  /* calculate hash over the received public key */
  if (iota_blake2b_sum(recv_pubkey,
                       sizeof(recv_pubkey),
                       pubkey_buffer_hash,
                       sizeof(pubkey_buffer_hash)) != 0) {
    printf("[%s:%d] get pubkey hash failed\n", __func__, __LINE__);
    return -1;
  }
#ifdef IOTA_L2_MSG_DEBUG
  printf("Hash calculated over the public key: ");
  dump_hex_str(pubkey_buffer_hash, CRYPTO_BLAKE2B_HASH_BYTES);
#endif /* IOTA_L2_MSG_DEBUG */

#ifdef IOTA_L2_MSG_DEBUG
  printf("Index received as parameter: ");
  dump_hex_str(index, ED25519_ADDRESS_BYTES);
#endif /* IOTA_L2_MSG_DEBUG */

  /* compare the hash of the received public key with the index:
   *   they must be the same to prove the channel ownership */
  if (memcmp(pubkey_buffer_hash, index, sizeof(pubkey_buffer_hash)) != 0) {
    printf("[%s:%d] comparing hash(pubkey)=index failed\n", __func__, __LINE__);
    return -1;
  }
#ifdef IOTA_L2_MSG_DEBUG
  else {
    printf("Channel ownership successfully verified: [hash(pubkey)=index] !\n");
  }
#endif /* IOTA_L2_MSG_DEBUG */

#ifdef L2SEC_WITH_STSAFE
  /* move the buffer index ahead */
  i += ED_SIGNATURE_BYTES;

  /* Get AuthSign from data */
  byte_t recv_auth_sign[STSAFEA_SIGN_ECDSA_SHA256_LENGTH];
  memcpy(recv_auth_sign, buffer->data + i, STSAFEA_SIGN_ECDSA_SHA256_LENGTH);
  //printf("AuthSign: "); dump_hex_str(recv_auth_sign, STSAFEA_SIGN_ECDSA_SHA256_LENGTH);

  /* Verify AuthSign */
  printf("STSAFE: Authentication Signature - Verify ");
  if (stsafe_AuthSign_verify(recv_auth_sign, buffer->data, i) != 0) {
    printf("[%s:%d] AuthSign verification failed\n", __func__, __LINE__);
    return -1;
  }
  printf("-> OK\n");
  /* */
#endif /* L2SEC_WITH_STSAFE */

#ifdef IOTA_L2_MSG_DEBUG
  printf("\nEXITING function: %s\n\n", __func__);
#endif /* IOTA_L2_MSG_DEBUG */

  return 0;
}

//
// =========================== L1 helper functions ======================
// 
/**
 * @brief  Send IOTA indexation message
 * @param[in]   app_data Message payload
 * @param[in]   index Message index
 * @param[out]  msg_id Message id
 * @retval 0 on success
 */
static int send_l1_message(byte_buf_t* app_data, byte_t* index, char* msg_id)
{

#ifdef IOTA_L1_MSG_DEBUG
  printf("\nENTERED function: %s\n", __func__);
#endif /* IOTA_L1_MSG_DEBUG */

  int err = 0;
  int rtx = 0;

  iota_client_conf_t ctx = {.host = NODE_HOST, .port = NODE_PORT, .use_tls = NODE_USE_TLS};
  res_send_message_t res;
  memset(&res, 0, sizeof(res_send_message_t));

  char data_buf[MAX_DATA_HEX_LENGTH];
  char index_buf[MAX_INDEX_HEX_LENGTH];
  memset(data_buf, 0, MAX_DATA_HEX_LENGTH);
  memset(index_buf, 0, MAX_INDEX_HEX_LENGTH);

  /* copy app_data and index into char buffer after conversion to hex strings, 
   *   as strings as input parameters are required by send_indexation() */
  bin_2_hex(app_data->data, app_data->len, data_buf, sizeof(data_buf));
  bin_2_hex(index, ED25519_ADDRESS_BYTES, index_buf, sizeof(index_buf));

  /*
   * if an error is encountered during connection setup or data sending,
   * the application will try to send the message again up to
   * MAX_L1_RETRY times
   */
  do {
    err = send_indexation_msg(&ctx, index_buf, data_buf, &res);

    if (res.is_error) {
      printf("[%s:%d]: ERROR response: %s\n", __func__, __LINE__, res.u.error->msg);
      res_err_free(res.u.error);
      return -1;
    }

    rtx++;
    if (err < 0) {
      printf("[%s:%d]: Message rtx #%d\n", __func__, __LINE__, rtx);
    }
  } while ( (err < 0) && (rtx <= MAX_L1_RETRY) );

  if (err < 0) {
    printf("[%s:%d]: ERROR send indexation failed!\n", __func__, __LINE__);
    return -1;
  } else if (err == 0){
    printf("Sent message - ID: %s\n", res.u.msg_id);
    printf("Sent message - index: %s\n", index_buf);
#ifdef IOTA_L1_MSG_DEBUG
    printf("\nSent message - data as hex encoded string: %s\n", data_buf);
    printf("\n");
#endif /* IOTA_L1_MSG_DEBUG */

    /* copy the message ID to be returned to the caller */
    strcpy(msg_id, res.u.msg_id);
  }
#ifdef IOTA_L1_MSG_DEBUG
  printf("\nEXITING function: %s\n\n", __func__);
#endif /* IOTA_L1_MSG_DEBUG */

  return 0;
}

/**
 * @brief  Get IOTA message by message id
 * @param[in]   msg_id Message id
 * @param[out]  data Message payload
 * @param[out]  index Message index
 * @retval 0 on success
 */
static int get_msg_by_id (char *msg_id, char *data, char *index)
{

#ifdef IOTA_L1_MSG_DEBUG
  printf("\nENTERED function: %s\n", __func__);
#endif /* IOTA_L1_MSG_DEBUG */

  int err = 0;
  iota_client_conf_t ctx = {.host = NODE_HOST, .port = NODE_PORT, .use_tls = NODE_USE_TLS};
  res_message_t* msg = res_message_new();

  // IOTA L1 API call
  get_message_by_id(&ctx, msg_id, msg);

#ifdef IOTA_L1_MSG_DEBUG
  printf("\nGetting message by ID (passed parameter): '%s'\n", msg_id);
#endif /* IOTA_L1_MSG_DEBUG */

  err = -1;
  if(msg->is_error) {
	  printf("[%s:%d]: API ERROR response: %s\n", __func__, __LINE__, msg->u.error->msg);
  }
  if(msg->u.msg->type == MSG_PAYLOAD_INDEXATION) {
	  err = 0;

      byte_buf_t* buf_data = NULL;
      byte_buf_t* buf_index = NULL;
      payload_index_t* idx = msg->u.msg->payload;

      buf_index = byte_buf_hex2str(idx->index);
      memcpy(index, buf_index->data, buf_index->len);
      index[buf_index->len] = '\0';

      buf_data = byte_buf_hex2str(idx->data);
      memcpy(data, buf_data->data, buf_data->len);
      data[buf_data->len] = '\0';

#ifdef IOTA_L1_MSG_DEBUG
      printf("Found message - index: '%s'\n", index);
      printf("Found message - data: '%s'\n", data);
#endif /* IOTA_L1_MSG_DEBUG */
  } else {
	  printf("[%s:%d]: ERROR message type: unknown instead of indexation\n", __func__, __LINE__);
  }

  res_message_free(msg);

  #ifdef IOTA_L1_MSG_DEBUG
    printf("\nEXITING function: %s\n\n", __func__);
  #endif /* IOTA_L1_MSG_DEBUG */


  return err;
}

/**
 * @brief  Get data and index form IOTA retrieved by message id
 * @param[in]   msg_id Message id
 * @param[out]  app_data Message payload
 * @param[out]  index Message index
 * @retval 0 on success
 */
static int get_l1_message_by_id (char *msg_id, byte_buf_t* app_data, byte_t* index)
{

#ifdef IOTA_L1_MSG_DEBUG
  printf("\nENTERED function: %s\n", __func__);
#endif /* IOTA_L1_MSG_DEBUG */

  int err = 0;

  char received_data_string[MAX_DATA_HEX_LENGTH];
  char received_index_string[MAX_INDEX_HEX_LENGTH];
  memset(received_data_string, 0, MAX_DATA_HEX_LENGTH);
  memset(received_index_string, 0, MAX_INDEX_HEX_LENGTH);

  // FIXME: check error, if any stop execution and return
  err = get_msg_by_id(msg_id, received_data_string, received_index_string);

#ifdef IOTA_L1_MSG_DEBUG
  printf("Received hex encoded index: %s\n", received_index_string);
  printf("Received hex encoded data: %s\n", received_data_string);
#endif /* IOTA_L1_MSG_DEBUG */

  // FIXME
  // IMPORTANT NOTE: the received 'index' (i.e. received_index_string) is not used
  //                 as it gets dirty with part of the received data
  //                 within the get_msg_by_id() function, soon after
  //                 the memcpy related to the received data 
  //                 It should used to check that it is the same as the 
  //                 requested index 

  /* copy app_data and index into char buffer after conversion to hex strings, 
   *   as strings as input parameters are required by send_indexation() */

  // FIXME: add error checking for byte_buf_reserve()
  byte_buf_reserve(app_data, strlen(received_data_string) / 2);
  app_data->len = strlen(received_data_string) / 2;
  hex_2_bin(received_data_string,
            strlen(received_data_string),
            app_data->data,
            app_data->len);

  hex_2_bin(received_index_string,
            strlen(received_index_string),
            index,
            ED25519_ADDRESS_BYTES);

#ifdef IOTA_L1_MSG_DEBUG
  printf("Received hex unencoded index: ");
  dump_hex_str(index, ED25519_ADDRESS_BYTES);
  printf("Received hex unencoded L2Sec wrapped data:\n");
  byte_buf_print(app_data);
#endif /* IOTA_L1_MSG_DEBUG */

#ifdef IOTA_L1_MSG_DEBUG
  printf("\nEXITING function: %s\n\n", __func__);
#endif /* IOTA_L1_MSG_DEBUG */

  return err;
}

//
// ======================= Example core helper functions ======================
//
/**
 * @brief  Retrieve sensor data to be included in the message payload
 * @param[out]  data_to_send Data retrieved reading sensors
 * @param[in]   n_iter Iteration number
 * @retval 0 on success
 */
static int get_app_data (byte_buf_t* data_to_send, int n_iter)
{
  char str_buffer[STR_BUFFER_SIZE];


  snprintf(str_buffer, STR_BUFFER_SIZE, "%s Msg. no. %d -- ", MSG_DATA, n_iter + 1);
  /* append the header of the application data to the buffer w/o string term */
  byte_buf_append(data_to_send, (byte_t const*)str_buffer, strlen(str_buffer));

  /* get sensor data from the board */
  if(sensor_data_to_json(str_buffer, STR_BUFFER_SIZE) != 0) {
    printf("Sensor data JSON formatting failed\n");
    snprintf(str_buffer, STR_BUFFER_SIZE, "%s", "ERROR - sensor_data_to_json");
  }

  /* append the sensor data to the buffer with the string terminator */
  byte_buf_append(data_to_send, (byte_t const*)str_buffer, strlen(str_buffer) + 1);

  return 0;
}


//
// =========================== Example core functions ======================
//
/**
 * @brief  Send L2Sec message
 * @param[out]  index Message index
 * @param[out]  seed Seed used to generate index
 * @param[out]  next_seed Seed for the next iteration
 * @param[in]   n_iter Iteration number
 * @retval 0 on success
 */
static int example_send_l2sec_protected_msg(char *msg_id,
                                            byte_t *index,
                                            byte_t *seed,
                                            byte_t *next_seed,
                                            int n_iter)
{
  int ret = 0;
  /*
   * wrap application data 
   */

  /* allocate new buffer for application data */ 
  byte_buf_t* data = NULL;
  data = byte_buf_new();

  /* allocate new buffer for the L2-wrapped application data being returned */ 
  byte_buf_t* l2_wrapped_data = NULL;
  l2_wrapped_data = byte_buf_new();

  /* append application data to the buffer */
  get_app_data(data, n_iter);
  printf("Application data message (payload) to be wrapped by L2Sec: \n'%s'\n", data->data);
  printf("Payload Size (bytes): %zu\n", data->len);

  /* wrap application data into an L2 payload */
  /*   note: index is returned to call the send_indexation() function */

  /* switch on LED2 before wrapping */
  BSP_LED_On(LED_GREEN);

  iota_l2sec_wrap (n_iter, seed, data, l2_wrapped_data, next_seed, index);
  printf("L2Sec Wrapped data (bytes): %zu\n", l2_wrapped_data->len);

#ifdef IOTA_L2SEC_MSG_EXAMPLE_DEBUG
  printf("Returned index from local iota_l2sec_wrap: ");
  dump_hex_str(index, ED25519_ADDRESS_BYTES);
#endif /*IOTA_L2SEC_MSG_EXAMPLE_DEBUG */

  /* switch off LED2 an toggle for 1s before sending */
  BSP_LED_Off(LED_GREEN);
  for (int i = 0; i < 10; i++)
  {
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(100);
  }
  BSP_LED_On(LED_GREEN);

  // Send L1 message
  printf ("Sending message... ");
  ret = send_l1_message(l2_wrapped_data, index, msg_id);
  printf ("Message Sent.\n");

  /* switch off LED2 after sending */
  BSP_LED_Off(LED_GREEN);
  
  /* deallocate buffers */
  byte_buf_free(data);
  byte_buf_free(l2_wrapped_data);

  return ret;
}

/**
 * @brief  Retrieve and decode L2Sec message by message Id
 * @param[in]  msg_id Message id
 * @param[in]  index Index of the current messae
 * @param[in]  next_index Seed for the next iteration
 * @param[in]  n_iter Iteration number
 * @retval None
 */
static void example_receive_l2sec_protected_msg_by_id(char *msg_id,
                                                      byte_t *index,
                                                      byte_t *next_index)
{
  /*
   * unwrap application data 
   */

  /* allocate new buffer for the received L2-wrapped application data */ 
  byte_buf_t* received_l2_wrapped_data = NULL;
  received_l2_wrapped_data = byte_buf_new();

  /* allocate new buffer for the L2-unwrapped application data */ 
  byte_buf_t* received_data = NULL;
  received_data = byte_buf_new();

  /* allocate new buffer for the received index */ 
  byte_t received_index[ED25519_ADDRESS_BYTES];

  // FIXME: check error, if any stop execution and return
  get_l1_message_by_id(msg_id, received_l2_wrapped_data, received_index);

  // TODO: check that received_index is the same as index; at the moment
  //       received_index gets dirty - issue to be fixed


  /* unwrap application data from an L2 payload */
  /*   note: next_index is returned for the second iteration of sending
             data */
  iota_l2sec_unwrap (received_l2_wrapped_data, index, 
		     received_data, next_index);

#ifdef IOTA_L2SEC_MSG_EXAMPLE_DEBUG
  printf("Returned next_index: ");
  dump_hex_str(next_index, ED25519_ADDRESS_BYTES);
#endif /*IOTA_L2SEC_MSG_EXAMPLE_DEBUG */

  printf("Returned L2Sec protected application data:\n'%s'\n", received_data->data);

  /* deallocate buffers */
  byte_buf_free(received_data);
  byte_buf_free(received_l2_wrapped_data);
}

/**
 * @brief  Search for L2Sec message by index
 * @param[in]  index Index of the current messae
 * @param[in]  next_index Seed for the next iteration
 * @param[in]  poll Flag telling if the receiver is in polling mode or not
 * @retval 0 on success
 */
static int example_receive_l2sec_protected_msg_by_index(byte_t *index,
                                                        byte_t *next_index,
                                                        int poll)
{
#ifdef IOTA_L2SEC_MSG_EXAMPLE_DEBUG
  printf("\nENTERED function: %s\n", __func__);
#endif /* IOTA_L2SEC_MSG_EXAMPLE_DEBUG */

#ifdef IOTA_L2SEC_MSG_EXAMPLE_DEBUG
  printf("\nChannel: searching for messages with index: ");  
  dump_hex_str(index, ED25519_ADDRESS_BYTES);
#endif /* IOTA_L2SEC_MSG_EXAMPLE_DEBUG */

  /* copy into char buffer after conversion to hex strings, 
   *   as string as input parameter is required by find_message_by_index() */
  char index_buf[MAX_INDEX_HEX_LENGTH];
  memset(index_buf, 0, sizeof(index_buf));
  bin_2_hex(index, ED25519_ADDRESS_BYTES, index_buf, sizeof(index_buf));

  /* call to IOTA L1 API function */
  int err = 0;
  iota_client_conf_t ctx = {.host = NODE_HOST, .port = NODE_PORT, .use_tls = NODE_USE_TLS};
  res_find_msg_t* res;
repeat:
  res = res_find_msg_new();
  err = find_message_by_index(&ctx, index_buf, res);

  if (res->is_error) {
    printf("ERROR response: %s\n", res->u.error->msg);
    res_err_free(res->u.error);
    return -1;
  }

  if (err) {
    printf("ERROR - find message by index failed!\n");
    return -1;
  } 
  else {
    int count = res->u.msg_ids->count;
    printf("Found >> %d << message(s) with the index: \n", count);
    dump_hex_str(index, ED25519_ADDRESS_BYTES);

    if (count != 1){
      /* no message to get and parse, exiting for nopoll mode... */
      /* or too many messages, exiting... */
      if (count == 0 && poll == READ_MODE_POLL){
        printf("Waiting 5 second before polling again for messages...\n\n");
	goto repeat;
      }
      /* SRA: receive also multiple messages with the same index */
      if (count == 0) {
        res_find_msg_free(res);
        return -1;
      }
    }

    char **p = NULL;

    // print the list of found message indexes
    p = (char**)utarray_next(res->u.msg_ids->msg_ids,p);
    while (p != NULL) {
      printf("Message ID: %s\n",*p);
      p = (char**)utarray_next(res->u.msg_ids->msg_ids,p);
    }

    // actually get each message by ID and print the content
    int i = 0;
    p = (char**)utarray_next(res->u.msg_ids->msg_ids,p);
    while (p != NULL) {
      /* SRA: parse just one message with a given index
         skip others
       */
      if (i > 0) {
        break;
      }

      example_receive_l2sec_protected_msg_by_id(*p, index, next_index);
      p = (char**)utarray_next(res->u.msg_ids->msg_ids,p);

      i++;
    }

    res_find_msg_free(res);
    return 0;
  }

#ifdef IOTA_L2SEC_MSG_EXAMPLE_DEBUG
  printf("\nEXITING function: %s\n", __func__);
#endif /* IOTA_L2SEC_MSG_EXAMPLE_DEBUG */
}

/**
 * @brief  Send L2Sec message stream
 * @param[out]  first_index First index of the message chain
 * @retval None
 */
static void example_send_l2sec_protected_stream(int n_messages, byte_t *first_index)
{
  int ret = 0;

#ifdef IOTA_L2SEC_MSG_EXAMPLE_DEBUG
  printf("\nENTERED function: %s\n", __func__);
#endif /* IOTA_L2SEC_MSG_EXAMPLE_DEBUG */

  // FIXME: check type (char?) and length of allocation: is it bin o string?
  char msg_id[IOTA_MESSAGE_ID_HEX_BYTES + 1]; 

  byte_t seed[IOTA_SEED_BYTES];
  memset(seed, 0, IOTA_SEED_BYTES);

  byte_t next_seed[IOTA_SEED_BYTES];
  memset(next_seed, 0, IOTA_SEED_BYTES);

  byte_t index[ED25519_ADDRESS_BYTES];
  memset(index, 0, ED25519_ADDRESS_BYTES);

  uint32_t n_iter;
  uint32_t n_max_msgs;


  printf("\n\n========================= CHANNEL: SENDING MESSAGES =========================\n\n");

  if(n_messages < 0) {
    n_messages = N_MESSAGES;
    printf("\n\nA negative value has been entered. Message number set to default %d \n\n", n_messages);
  }
  if (n_messages == 0) { /* (almost infinite loop) */
    n_max_msgs = 0xFFFFFFFF;
#ifdef __ARMCC_VERSION
    printf("\n\nChannel: >> (hex) %x << message(s) will be sent (almost infinite loop) \n\n", n_max_msgs);
#else
    printf("\n\nChannel: >> (hex) %lx << message(s) will be sent (almost infinite loop) \n\n", n_max_msgs);
#endif /* __ARMCC_VERSION */
  }
  else {
    n_max_msgs = n_messages;
#ifdef __ARMCC_VERSION
    printf("\n\nChannel: >> %u << message(s) will be sent \n\n", n_max_msgs);
#else
    printf("\n\nChannel: >> %lu << message(s) will be sent \n\n", n_max_msgs);
#endif /* __ARMCC_VERSION */
  } 

  for (n_iter = 0; n_iter < n_max_msgs; n_iter++) {

#ifdef __ARMCC_VERSION
    printf("\n\nChannel: sending message # %u (hex: %x) ...\n\n", n_iter + 1, n_iter + 1);
#else
    printf("\n\nChannel: sending message # %lu (hex: %lx) ...\n\n", n_iter + 1, n_iter + 1);  
#endif /* __ARMCC_VERSION */
    ret = example_send_l2sec_protected_msg(msg_id, index, seed, next_seed, n_iter);
    if (ret) {
      printf("\nEncountered a network problem...message stream suspended\n");
      break;
    }

#ifdef IOTA_L2SEC_MSG_EXAMPLE_DEBUG
  printf("Example channel function:\n");
  printf("Seed (before assigning next_seed): ");
  dump_hex_str(seed, IOTA_SEED_BYTES);
  printf("Next_seed: ");
  dump_hex_str(next_seed, IOTA_SEED_BYTES);
#endif /*IOTA_L2SEC_MSG_EXAMPLE_DEBUG */

    /* use the returned next_seed as seed for the next iteration, 
     * to build the chain through the index and next_index generated
     * inside the called functions */
    memcpy(seed, next_seed, IOTA_SEED_BYTES);

    if (n_iter == 0) {
      /* save the index of the first message to return it to the caller */
      memcpy(first_index, index, ED25519_ADDRESS_BYTES);

      printf("\nIndex of the first message in the channel: ");
      dump_hex_str(first_index, ED25519_ADDRESS_BYTES);
    }
  }
#ifdef IOTA_L2SEC_MSG_EXAMPLE_DEBUG
  printf("\nEXITING function: %s\n", __func__);
#endif /* IOTA_L2SEC_MSG_EXAMPLE_DEBUG */
}

/**
 * @brief  Receive L2Sec message stream
 * @param[in]  index First index of the message chain
 * @param[in]  poll Flag telling if the receiver is in polling mode or not
 * @retval None
 */
static void example_receive_l2sec_protected_stream(byte_t* index, int poll)
{

#ifdef IOTA_L2SEC_MSG_EXAMPLE_DEBUG
  printf("\nENTERED function: %s\n", __func__);
#endif /* IOTA_L2SEC_MSG_EXAMPLE_DEBUG */

  byte_t next_index[ED25519_ADDRESS_BYTES];
  memset(next_index, 0, ED25519_ADDRESS_BYTES);

  int n_iter = 0;
  int ret = 0;

  printf("\n\n========================== CHANNEL: RECEIVING MESSAGES ==========================\n\n");

  printf("\nChannel: starting searching for index: ");  
  dump_hex_str(index, ED25519_ADDRESS_BYTES);

  do {
    printf("\n\nChannel: receiving message # %d...\n\n", ++n_iter);  
    ret = example_receive_l2sec_protected_msg_by_index(index, next_index, poll);

    /* use the returned next_index as index for the next iteration */
    memcpy(index, next_index, ED25519_ADDRESS_BYTES);
  } while (ret == 0);

  printf("\nChannel: message # %d not found...\n\n", n_iter);  

#ifdef IOTA_L2SEC_MSG_EXAMPLE_DEBUG
  printf("\nEXITING function: %s\n", __func__);
#endif /* IOTA_L2SEC_MSG_EXAMPLE_DEBUG */
}


/* Exported function definition ----------------------------------------------*/

//
// ======================== Main and related functions ======================
//
/**
 * @brief  Entry function for:
 *         1. Sending an L2Sec message stream composed of \p n_messages messages
 *            If \p n_messages is set to 0 then an infinite stream is generated
 *            The latter option should be used when we are in sending only mode
 *         2. Receiving an L2Sec stream in #READ_MODE_NOPOLL (otherwise
 *            the function will never return)
 * @param  n_messages Number of messages to be sent
 * @retval None
 */
void example_send_receive_l2sec_protected_stream(int n_messages)
{
#ifdef L2SEC_WITH_STSAFE
  int ret;

  ret = stsafe_l2sec_init();
  if(ret != 0) {
    printf("\nEXITING function: %s due to STSAFE setup error\n", __func__);
    return;
  }
#endif /* L2SEC_WITH_STSAFE */

  /* allocate buffer for the first index in the stream chain */
  byte_t first_index[ED25519_ADDRESS_BYTES];

  // TODO: add error management
  example_send_l2sec_protected_stream(n_messages, first_index);
  example_receive_l2sec_protected_stream(first_index, READ_MODE_NOPOLL);
}

/**
 * @brief  Entry function for:
 *         Sending an L2Sec message stream composed of \p n_messages messages
 *         If \p n_messages is set to 0 then an infinite stream is generated.
 *         The latter option should be used when we are in sending only mode.
 * @param  n_messages Number of messages to be sent
 * @retval None
 */
void example_only_send_l2sec_protected_stream(int n_messages)
{
#ifdef L2SEC_WITH_STSAFE
  int ret;

  ret = stsafe_l2sec_init();
  if(ret != 0) {
    printf("\nEXITING function: %s due to STSAFE setup error\n", __func__);
    return;
  }
#endif /* L2SEC_WITH_STSAFE */

  /* allocate buffer for the first index in the stream chain */
  byte_t first_index[ED25519_ADDRESS_BYTES];

  // TODO: add error management
  example_send_l2sec_protected_stream(n_messages, first_index);
}

/**
 * @brief  Entry function for:
 *         Receiving an L2Sec stream in #READ_MODE_POLL or in #READ_MODE_NOPOLL
 * @param[in]  index_str First index of the stream
 * @param[in]  poll Flag telling if the receiver is in polling mode or not
 * @retval None
 */
void example_only_receive_l2sec_protected_stream(char *index_str, int poll)
{

  /* allocate buffer for the first index in the stream chain */
  byte_t index[ED25519_ADDRESS_BYTES];

  hex_2_bin(index_str, strlen(index_str), index, ED25519_ADDRESS_BYTES);

  // TODO: add error management
  example_receive_l2sec_protected_stream(index, poll);
}
