// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client/api/v1/get_message.h"
#include "client/api/v1/send_message.h"

#include "iota_conf.h"

/* Constants -----------------------------------------------------------------*/
#define XOR_KEY 'S'

/* Private functions ---------------------------------------------------------*/
/**
 * @brief Simple encrypt/decrypt for this example
 *
 * @param src A string for decrypt/encrypt
 * @param dest A buffer stores the decrypt/encrypt data
 * @param dest_len the buffer length
 * @retval int 0 on succecc
 */
static int xor_encrypt_decrypt(char const *src,
                               size_t src_len,
                               char *dest,
                               size_t dest_len)
{
  if (src_len > dest_len - 1) {
    // dest buffer too small
    return -1;
  }

  for (size_t i = 0; i < src_len; i++) {
    dest[i] = src[i] ^ XOR_KEY;
  }
  dest[src_len] = '\0';
  return 0;
}

/**
 * @brief Validate decrypted message
 *
 * @param ctx A client context
 * @param src A string for decrypt/validate
 * @param msg_id A message ID
 * @return int 0 on succecc
 */
static int validation(iota_client_conf_t *ctx,
                      char const *const src,
                      char msg_id[])
{
  char decrypted[100] = {0};

  res_message_t *msg = res_message_new();
  if (msg == NULL) {
    return -1;
  }

  // get message via the message ID
  if (get_message_by_id(ctx, msg_id, msg) != 0) {
    res_message_free(msg);
    return -1;
  }

  if (msg->is_error == true) {
    printf("Error: %s\n", msg->u.error->msg);
  } else {
    // check payload type
    if (msg->u.msg->type != MSG_PAYLOAD_INDEXATION) {
      printf("Not an indexation payload\n");
      res_message_free(msg);
      return -1;
    }

    // get indexation payload
    payload_index_t *index = (payload_index_t *)msg->u.msg->payload;

    printf("Data on the Tangle: %s\n", index->data->data);
    // index->data is hex string, it needs to convert to binary.
    hex_2_bin((char *)index->data->data, strlen((char *)index->data->data), (byte_t *)decrypted, sizeof(decrypted));

    // decrypt data
    xor_encrypt_decrypt(decrypted, strlen(decrypted), decrypted, sizeof(decrypted));
    printf("Decrypted data: %s\n", decrypted);

    // data matching
    if (memcmp(src, decrypted, strlen(src)) == 0) {
      printf("Matched\n");
    } else {
      printf("Miss matched\n");
    }
  }
  res_message_free(msg);
  return 0;
}

/* Exported functions --------------------------------------------------------*/
int send_enc_data(void)
{
  char const *const data = "data to be encrypted";
  char encrypted[100] = {0};

  // data encryption
  xor_encrypt_decrypt(data, strlen(data), encrypted, sizeof(encrypted));

  // send encripted data
  iota_client_conf_t ctx = {.host = NODE_HOST, .port = NODE_HOST_PORT, .use_tls = NODE_USE_TLS};
  indexation_t *idx = NULL;
  core_message_t *msg = NULL;
  res_send_message_t msg_res;
  memset(&msg_res, 0, sizeof(res_send_message_t));
  idx = indexation_create("iota.c\xF0\x9F\xA6\x8B", (byte_t *)encrypted, strlen(encrypted));
  msg = core_message_new();
  msg->payload = idx;
  msg->payload_type = MSG_PAYLOAD_INDEXATION;  // indexation playload

  if (send_core_message(&ctx, msg, &msg_res) != 0) {
    printf("send message failed\n");
    goto err;
  }

  if (msg_res.is_error == true) {
    printf("Error: %s\n", msg_res.u.error->msg);
    goto err;
  }

  printf("Message ID: %s\nEncrypted data: %s\n", msg_res.u.msg_id, encrypted);

  // validating data
  validation(&ctx, data, msg_res.u.msg_id);

  core_message_free(msg);
  return 0;

err:
  core_message_free(msg);
  return -1;
}
