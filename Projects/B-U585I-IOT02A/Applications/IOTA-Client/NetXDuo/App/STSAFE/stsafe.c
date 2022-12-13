/**
  ******************************************************************************
  * @file   stsafe.c
  * @author SRA/Central LAB
  * @brief  This file provides functions for testing some STSAFE functionalities
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "b_u585i_iot02a.h"
#include "stsafea_core.h"

/* *********************** STSAFE-L2SEC Integration ************************ */
#include "mbedtls/sha256.h"
#include "mbedtls/x509_crt.h"
#include "core/types.h"
#include "core/utils/byte_buffer.h"
/* *********************** STSAFE-L2SEC Integration ************************ */

/* Constants -----------------------------------------------------------------*/
#define ENVELOPE_SIZE           (1*8)//(60*8)
#define WRAP_RESPONSE_SIZE      (ENVELOPE_SIZE+8)

#define STSAFE_ENABLE_DEBUG

#ifdef STSAFE_ENABLE_DEBUG
#define printf_stsafe_dbg printf
#else
#define printf_stsafe_dbg(...)
#endif


/* *********************** STSAFE-L2SEC Integration ************************ */
#define LINKS_IOTA_L2SEC_PSK "AC88DFA4DEAAE33E0135DFF4A6BB678FA7FFDC10869ADC6E6D38DDCBC90CAC88"
#define L2SEC_PSK_SIZE              (32)
#define L2SEC_PSK_NVM_ZONE           (1)
#define L2SEC_PSK_NVM_OFFSET         (0)
#define L2SEC_PSK_ENVELOPE_SIZE     (32)   // Must be multiple of 8 and < 480 bytes
#define L2SEC_PSK_WRAPPED_SIZE      (L2SEC_PSK_ENVELOPE_SIZE+8)

#define CERTSIZE_LEN                (4)

static uint8_t links_psk_wrapped[L2SEC_PSK_WRAPPED_SIZE] = {0};   // Wrapped PSK
/* *********************** STSAFE-L2SEC Integration ************************ */

/* Private variables ---------------------------------------------------------*/
static StSafeA_Handle_t stsafea_handle;
/* STSAFE MW requires a data buffer to send/receive bytes over the bus.
   For memory optimization reasons it's up to the application to allocate it,
   so that the application can deallocate it when STSAFE services are not required anymore */
static uint8_t a_rx_tx_stsafea_data [STSAFEA_BUFFER_MAX_SIZE];

/* Private function declarations ---------------------------------------------*/
static int stsafe_pairing(void);
static int GenerateUnsignedChallenge(StSafeA_Handle_t* handle,
                                     uint16_t size,
                                     uint8_t* random);

/* *********************** STSAFE-L2SEC Integration ************************ */
static int check_local_envelope_key(void);
static uint8_t stsafe_PSK_init(void);
static uint8_t stsafe_PSK_sec_load(uint8_t *psk);
static uint8_t stsafe_PSK_sec_store(uint8_t *inPSK);
static uint8_t stsafe_NVM_write(uint8_t inZone,
                                uint16_t inOffset,
                                uint8_t *inData,
                                uint16_t inDataLen);
static uint8_t stsafe_PSK_isStored(void);
static uint8_t stsafe_NVM_read(uint8_t inZone,
                               uint16_t inOffset,
                               uint16_t inAmount,
                               uint8_t *outData);
static uint8_t stsafe_PSK_wrap(uint8_t *inPSK, uint8_t *outWrappedPSK);
static uint8_t stsafe_cert_get_public_key(mbedtls_x509_crt *inCert,
                                          uint8_t *pubK_X,
                                          uint8_t *pubK_Y);
static uint8_t stsafe_cert_NVM_read(mbedtls_x509_crt *outSTSAFE_Cert);
static uint8_t stsafe_cert_NVM_getsize(uint16_t *outLenCertificate);
static uint8_t stsafe_PSK_deinit(void);
/* *********************** STSAFE-L2SEC Integration ************************ */

/* Private function definitions ----------------------------------------------*/



static int check_local_envelope_key(void)
{
  int32_t StatusCode = 0;
  StSafeA_LocalEnvelopeKeyTableBuffer_t LocalEnvelopeKeyTable;
  StSafeA_LocalEnvelopeKeyInformationRecordBuffer_t  LocalEnvelopeInfoSlot0, LocalEnvelopeInfoSlot1;


  printf_stsafe_dbg("\n\r Check local envelope key presence through STSAFE-A110");
  printf_stsafe_dbg("\n\r        => StSafeA_LocalEnvelopeKeySlotQuery");

  StatusCode = StSafeA_LocalEnvelopeKeySlotQuery(&stsafea_handle,
		  &LocalEnvelopeKeyTable, &LocalEnvelopeInfoSlot0,
		  &LocalEnvelopeInfoSlot1, STSAFEA_MAC_NONE);

  if ((StatusCode == 0 ) && (LocalEnvelopeKeyTable.NumberOfSlots != 0U) && (LocalEnvelopeInfoSlot0.SlotNumber == 0U) && (LocalEnvelopeInfoSlot0.PresenceFlag == 0U))
  {
	  printf_stsafe_dbg("\n\r Generate local envelope key");
	  printf_stsafe_dbg("\n\r        => StSafeA_GenerateLocalEnvelopeKey");

    StatusCode = (int32_t)StSafeA_GenerateLocalEnvelopeKey(&stsafea_handle,
    		STSAFEA_KEY_SLOT_0, STSAFEA_KEY_TYPE_AES_128,
			NULL, 0U, STSAFEA_MAC_NONE);
  }

  return StatusCode;
}


/* Perform pairing between Host MCU and STSAFE (set Host Keys)
 * Send host cipher key & host MAC key to STSAFE for pairing
 */
static int stsafe_pairing(void)
{
  int status_code = STSAFEA_INVALID_PARAMETER;
  StSafeA_HostKeySlotBuffer_t out;
  
  uint8_t K_CMAC_Host[32] = {
    0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77, 0x88, 0x88,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

  status_code = check_local_envelope_key();

  status_code = StSafeA_HostKeySlotQuery(&stsafea_handle, &out, STSAFEA_MAC_NONE);
  if (status_code == STSAFEA_OK)
  {
    if (out.HostKeyPresenceFlag == 0)
    {
    	status_code = StSafeA_PutAttribute(&stsafea_handle,           // STSAFE obj ptr
										   STSAFEA_TAG_HOST_KEY_SLOT, // Attribute tag
										   K_CMAC_Host,               // dataIn (Keys)
										   sizeof(K_CMAC_Host),       // dataInLen
										   STSAFEA_MAC_NONE);         // MAC
    } else {
    	status_code = STSAFEA_OK;
    }
  }

  printf_stsafe_dbg("\npairing_stsafe result (0 means success): %d\n", (int)status_code);

  return status_code;
}

/* Generate Unsigned Bytes Array */
static int GenerateUnsignedChallenge(StSafeA_Handle_t* handle,
                                     uint16_t size,
                                     uint8_t* random)
{
  STSAFEA_UNUSED_PTR(handle);

  if (random == NULL)
  {
    return (1);
  }

  uint16_t i;

  srand(HAL_GetTick());

  for (i = 0U; i < size; i++)
  {
    random[i] = (uint8_t)((uint16_t)rand() % 256U);
  }

  random[0] &= 0x7FU;

  return (0);
}

/* *********************** STSAFE-L2SEC Integration ************************ */
/* Initialize L2SEC-PSK into STSAFE-NVM */
static uint8_t stsafe_PSK_init(void)
{
  uint32_t StatusCode = STSAFEA_INVALID_PARAMETER;

  printf_stsafe_dbg("STSAFE: (demo-only) PSK is reset ");
  StatusCode = stsafe_PSK_deinit();
  printf_stsafe_dbg("-> OK\n");

  if(!stsafe_PSK_isStored()){
    printf_stsafe_dbg("STSAFE: PSK in STSAFE-NVM is empty\n");
    uint8_t tmp_plain_psk[L2SEC_PSK_SIZE] = {0};
    hex_2_bin(LINKS_IOTA_L2SEC_PSK, strlen(LINKS_IOTA_L2SEC_PSK),
              tmp_plain_psk, L2SEC_PSK_SIZE);
    
    // Secure Store = Wrap PSK + Store(Wrapped_PSK) on NVM
    printf_stsafe_dbg("STSAFE: Writing PSK to STSAFE-NVM ");
    StatusCode = stsafe_PSK_sec_store(tmp_plain_psk);
    printf_stsafe_dbg("-> OK\n");
  }

  // Secure Load = Load(Wrapped PSK) from NVM to RAM
  printf_stsafe_dbg("STSAFE: Loading Wrapped PSK from STSAFE-NVM to memory ");
  StatusCode = stsafe_PSK_sec_load(links_psk_wrapped);
  printf_stsafe_dbg("-> OK\n");

  return(StatusCode);
}

/* Fetch Wrapped PSK from STSAFE-NVM and load it into RAM */
static uint8_t stsafe_PSK_sec_load(uint8_t *psk)
{
  uint8_t StatusCode = STSAFEA_INVALID_PARAMETER;

  // Retrieve Wrapped_PSK from STSAFE-NVM
  StatusCode = stsafe_NVM_read(L2SEC_PSK_NVM_ZONE,
                                     L2SEC_PSK_NVM_OFFSET,
                                     L2SEC_PSK_WRAPPED_SIZE,
                                     psk);

  return(StatusCode);
}

/* Wrap plain PSK and store it into STSAFE-NVM */
static uint8_t stsafe_PSK_sec_store(uint8_t *inPSK)
{
  uint8_t StatusCode = STSAFEA_INVALID_PARAMETER;

  // 1. Wrap PSK
  uint8_t wrapped_PSK[L2SEC_PSK_WRAPPED_SIZE] = {0};
  StatusCode = stsafe_PSK_wrap(inPSK, wrapped_PSK);

  // 2. Store it to STSAFE-NVM
  StatusCode |= stsafe_NVM_write(L2SEC_PSK_NVM_ZONE,
                                       L2SEC_PSK_NVM_OFFSET,
                                       wrapped_PSK,
                                       L2SEC_PSK_WRAPPED_SIZE);

  return(StatusCode);
}

/* Write data into STSAFE-NVM */
static uint8_t stsafe_NVM_write(uint8_t inZone,
                                uint16_t inOffset,
                                uint8_t *inData,
                                uint16_t inDataLen)
{
  uint8_t StatusCode = STSAFEA_INVALID_PARAMETER;

  StSafeA_LVBuffer_t buff;
  buff.Data = inData;
  buff.Length = inDataLen;

  StatusCode = StSafeA_Update(&stsafea_handle,
                              STSAFEA_FLAG_TRUE,
                              0, 0, STSAFEA_AC_ALWAYS,
                              inZone, inOffset,
                              &buff,
                              STSAFEA_MAC_NONE);

  return(StatusCode);
}

/* Check if PSK is stored within STSAFE-NVM */
static uint8_t stsafe_PSK_isStored(void)
{ 
  // 1. Retrieve Wrapped PSK from STSAFE-NVM
  uint8_t tmp_wrapped_psk[L2SEC_PSK_WRAPPED_SIZE] = {0};
  uint8_t to_cmp[L2SEC_PSK_WRAPPED_SIZE] = {0};

  (void)stsafe_NVM_read(L2SEC_PSK_NVM_ZONE,
                              L2SEC_PSK_NVM_OFFSET,
                              L2SEC_PSK_WRAPPED_SIZE,
                              tmp_wrapped_psk);

   if (memcmp(tmp_wrapped_psk, to_cmp, L2SEC_PSK_WRAPPED_SIZE) == 0){
     return(0);  // all zeros, ie not init
   } else {
     return(1);  // something is stored
   }
}

/* Read data from STSAFE-NVM */
static uint8_t stsafe_NVM_read(uint8_t inZone,
                               uint16_t inOffset,
                               uint16_t inAmount,
                               uint8_t *outData)
{
  uint8_t StatusCode = STSAFEA_INVALID_PARAMETER;

  StSafeA_LVBuffer_t buff;
  buff.Data = outData;
  buff.Length = inAmount;

  StatusCode = StSafeA_Read(&stsafea_handle,
                            0, 0, STSAFEA_AC_ALWAYS,
                            inZone, inOffset,
                            buff.Length, buff.Length,
                            &buff,
                            STSAFEA_MAC_NONE);

  return(StatusCode);
}

/* Wrap the PSK into the envelope */
static uint8_t stsafe_PSK_wrap(uint8_t *inPSK, uint8_t *outWrappedPSK)
{
  uint8_t StatusCode = STSAFEA_INVALID_PARAMETER;

  StSafeA_LVBuffer_t LocalEnvelope;
  LocalEnvelope.Length = L2SEC_PSK_WRAPPED_SIZE;
  LocalEnvelope.Data = outWrappedPSK;

  StatusCode = StSafeA_WrapLocalEnvelope(&stsafea_handle,
                                         STSAFEA_KEY_SLOT_0,
                                         inPSK,
                                         L2SEC_PSK_ENVELOPE_SIZE,
                                         &LocalEnvelope,
                                         STSAFEA_MAC_HOST_CMAC,
                                         STSAFEA_ENCRYPTION_COMMAND);

  return(StatusCode);
}

/* Get ECDSA-PublicKey from Certificate */
static uint8_t stsafe_cert_get_public_key(mbedtls_x509_crt *inCert,
                                          uint8_t *pubK_X,
                                          uint8_t *pubK_Y)
{
  uint8_t StatusCode = STSAFEA_INVALID_PARAMETER;

  // 1. Get the Public Key
  mbedtls_ecp_point *q;
  q = &(mbedtls_pk_ec(inCert->pk)->Q);

  // 2. "Split" ECDSA Public Key Q in tuple (X,Y) - for ease of STSAFE data structures
  StatusCode = mbedtls_mpi_write_binary(&(q->X), pubK_X, STSAFEA_XYRS_ECDSA_SHA256_LENGTH);
  StatusCode |= mbedtls_mpi_write_binary(&(q->Y), pubK_Y, STSAFEA_XYRS_ECDSA_SHA256_LENGTH);

  return(StatusCode);
}

/* Read STSAFE Leaf Certificate from STSAFE-NVM */
static uint8_t stsafe_cert_NVM_read(mbedtls_x509_crt *outSTSAFE_Cert)
{
  uint8_t StatusCode = STSAFEA_INVALID_PARAMETER;

  // 1. Get the size of the certificate
  uint16_t CertificateSize = 0;
  StatusCode = stsafe_cert_NVM_getsize(&CertificateSize);

  // 2. Extract the whole certificate
  StSafeA_LVBuffer_t RawCertificate;
  uint8_t data_Certificate[CertificateSize];
  RawCertificate.Length = CertificateSize;
  RawCertificate.Data = data_Certificate;

  StatusCode = StSafeA_Read(&stsafea_handle,
                            0, 0, STSAFEA_AC_ALWAYS,
                            0, 0,
                            CertificateSize, CertificateSize,
                            &RawCertificate,
                            STSAFEA_MAC_NONE);

  // 3. Parse the certificate
  mbedtls_x509_crt_init(outSTSAFE_Cert);
  mbedtls_x509_crt_parse(outSTSAFE_Cert, RawCertificate.Data, CertificateSize);

  return(StatusCode);
}

/* Read Size of STSAFE Leaf Certificate from STSAFE-NVM */
static uint8_t stsafe_cert_NVM_getsize(uint16_t *outLenCertificate)
{
  uint8_t StatusCode = STSAFEA_INVALID_PARAMETER;

  uint16_t CertificateSize = 0;

  StSafeA_LVBuffer_t CertifSize;
  uint8_t data_CertifSize[CERTSIZE_LEN];
  CertifSize.Length = CERTSIZE_LEN;
  CertifSize.Data = data_CertifSize;

  StatusCode = StSafeA_Read(&stsafea_handle,
                            0, 0, STSAFEA_AC_ALWAYS,
                            0, 0,
                            CERTSIZE_LEN, CERTSIZE_LEN,
                            &CertifSize,
                            STSAFEA_MAC_NONE);

  if (StatusCode == 0) {
    switch(CertifSize.Data[1]){
    case 0x81U:
      CertificateSize = (uint16_t)CertifSize.Data[2] + 3U;
      break;
    case 0x82U:
      CertificateSize = (((uint16_t)CertifSize.Data[2]) << 8) + CertifSize.Data[3] + 4U;
      break;
    default:
      if (CertifSize.Data[1] < 0x81U) {
        CertificateSize = CertifSize.Data[1];
      }
      break;
    }
  }

  *outLenCertificate = CertificateSize;

  return(StatusCode);
}

/* Unset (zeroes) L2SEC-PSK into STSAFE-NVM */
static uint8_t stsafe_PSK_deinit(void)
{
  uint8_t StatusCode = STSAFEA_INVALID_PARAMETER;

  uint8_t zero_PSK[L2SEC_PSK_WRAPPED_SIZE] = {0};

  // Write all zeros in place of PSK
  StatusCode = stsafe_NVM_write(L2SEC_PSK_NVM_ZONE,
                                      L2SEC_PSK_NVM_OFFSET,
                                      zero_PSK,
                                      L2SEC_PSK_WRAPPED_SIZE);

  return(StatusCode);
}

/* *********************** STSAFE-L2SEC Integration ************************ */

/* Exported function definition ----------------------------------------------*/
/*************************** Initialisation ************************/
int stsafe_init(void)
{
  uint8_t i;
  int status_code;

  /* Create STSAFE-A's handle */
  status_code = StSafeA_Init(&stsafea_handle, a_rx_tx_stsafea_data);

  if (status_code == 0U)
  {
    printf("\n\rSTSAFE-A110 initialized successfully\n\r\n\r");
  }
  else
  {
    printf("\n\r\n\r\n\r** STSAFE-A110 NOT initialized. ** \n\r");

    for (i = 0U; i < 10U; i++)
    {
      (void)BSP_LED_Toggle(LED7);
      HAL_Delay(100);
    }
  }
  
  return status_code;
}

/*************************** Wrap Unwrap Envelope ************************/
int stsafe_wrap_unwrap(void)
{
  int status_code = 0;
  uint8_t MyData[ENVELOPE_SIZE] = {0};

  // WRAP variables
  StSafeA_LVBuffer_t LocalEnvelope;
  uint8_t data_LocalEnvelope [WRAP_RESPONSE_SIZE] = {0};
  LocalEnvelope.Length = WRAP_RESPONSE_SIZE;
  LocalEnvelope.Data = data_LocalEnvelope;

  // UNWRAP variables
  StSafeA_LVBuffer_t UnwrappedEnvelope;
  uint8_t data_UnwrappedEnvelope [ENVELOPE_SIZE] = {0};
  UnwrappedEnvelope.Length = ENVELOPE_SIZE;
  UnwrappedEnvelope.Data = data_UnwrappedEnvelope;

  // init test data
  // memset(MyData, 'x', ENVELOPE_SIZE);
  status_code = GenerateUnsignedChallenge(&stsafea_handle, ENVELOPE_SIZE, &MyData[0]);
  printf("\n\rstsafe_wrap_unwrap result (GenerateUnsignedChallenge - 0 means success): %d\n", status_code);
  if(status_code != STSAFEA_OK)
  {
    return status_code;
  }

  // The following two steps (if not already done at least once) are needed before doing Wrap and Unwrap
  // - pairing
  // - Envelope Key generation
  //status_code = stsafe_pairing();
  //if(status_code != STSAFEA_OK)
  //{
  //  return status_code;
  //}
  status_code = StSafeA_GenerateLocalEnvelopeKey(&stsafea_handle,
                                                 STSAFEA_KEY_SLOT_0,
                                                 STSAFEA_KEY_TYPE_AES_128,
                                                 NULL,
                                                 0U,
                                                 STSAFEA_MAC_NONE);
  printf("\n\rstsafe_wrap_unwrap result (StSafeA_GenerateLocalEnvelopeKey - 0 means success): %d\n", status_code);
  if(status_code != STSAFEA_OK)
  {
    return status_code;
  }

  /* Wrap local envelope using key slot as argument */
  status_code = StSafeA_WrapLocalEnvelope(&stsafea_handle,
                                          STSAFEA_KEY_SLOT_0,
                                          &MyData[0],
                                          ENVELOPE_SIZE,
                                          &LocalEnvelope,
                                          STSAFEA_MAC_HOST_CMAC,
                                          STSAFEA_ENCRYPTION_COMMAND);
  
  printf("\n\rstsafe_wrap_unwrap result (StSafeA_WrapLocalEnvelope - 0 means success): %d\n", status_code);
  if(status_code != STSAFEA_OK)
  {
    return status_code;
  }

  /* Unwrap local envelope using key in slot as argument */
  status_code = StSafeA_UnwrapLocalEnvelope(&stsafea_handle,
                                            STSAFEA_KEY_SLOT_0,
                                            LocalEnvelope.Data,
                                            LocalEnvelope.Length,
                                            &UnwrappedEnvelope,
                                            STSAFEA_MAC_HOST_CMAC,
                                            STSAFEA_ENCRYPTION_RESPONSE);

  printf("\n\rVerify unwrap local envelope is identical to initial generated envelope");
  if ((status_code == 0) && (memcmp(&MyData[0], &UnwrappedEnvelope.Data[0], ENVELOPE_SIZE) != 0))
  {
    status_code = (int32_t)~0U;
  }
  printf("\n\rstsafe_wrap_unwrap result (StSafeA_UnwrapLocalEnvelope - 0 means success): %d\n", status_code);
  
  return status_code;
}


/* ************************************************************************* */
/* *********************** STSAFE-L2SEC Integration ************************ */
/* ************************************************************************* */

/* Generate random data using TRNG of STSAFE */
int stsafe_GenRandom(uint8_t *outRnd, uint8_t inLen)
{
  int StatusCode = STSAFEA_INVALID_PARAMETER;

  if (outRnd == NULL)
  {
    return (StatusCode);
  }

//  printf("  Generate a %d bytes random number\n", inLen);
//  printf("    => Use StSafeA_GenerateRandom API\n\n");

  StSafeA_LVBuffer_t outRndSTSAFE;
  outRndSTSAFE.Data = outRnd;

  StatusCode = StSafeA_GenerateRandom(&stsafea_handle,
                                      STSAFEA_EPHEMERAL_RND,
                                      inLen,
                                      &outRndSTSAFE,
                                      STSAFEA_MAC_NONE);
  //printf("Use StSafeA_GenerateRandom API    => ret: %d\n\n", StatusCode);
  return (StatusCode);
}


/* Create the ECDSA Signature with STSAFE using the PrivateKey of the Leaf Certificate */
int stsafe_AuthSign_sign(uint8_t *inData, size_t inDataLen, uint8_t *outSign)
{
  int StatusCode = STSAFEA_INVALID_PARAMETER;

  // 1. Hash the data
  uint8_t data_Hash[STSAFEA_SHA_256_LENGTH] = {0};
  mbedtls_sha256(inData, inDataLen, data_Hash, 0);
  //printf("dataHash: "); dump_hex_str(data_Hash, STSAFEA_SHA_256_LENGTH);   // TODO debug

  // 2. Sign the data
  StSafeA_LVBuffer_t SignR;
  uint8_t data_SignR [STSAFEA_XYRS_ECDSA_SHA256_LENGTH];
  SignR.Length = STSAFEA_XYRS_ECDSA_SHA256_LENGTH;
  SignR.Data = data_SignR;
  StSafeA_LVBuffer_t SignS;
  uint8_t data_SignS [STSAFEA_XYRS_ECDSA_SHA256_LENGTH];
  SignS.Length = STSAFEA_XYRS_ECDSA_SHA256_LENGTH;
  SignS.Data = data_SignS;

  StatusCode = StSafeA_GenerateSignature(&stsafea_handle,
                                         STSAFEA_KEY_SLOT_0,
                                         data_Hash, STSAFEA_SHA_256,
                                         STSAFEA_XYRS_ECDSA_SHA256_LENGTH,
                                         &SignR, &SignS,
                                         STSAFEA_MAC_NONE, STSAFEA_ENCRYPTION_NONE);

  // 3. outSign = [R, S]
  memcpy(outSign, data_SignR, SignR.Length);
  memcpy(outSign+32, data_SignS, SignS.Length);
  //printf("AuthSign: "); dump_hex_str(outSign, STSAFEA_SIGN_ECDSA_SHA256_LENGTH);   // TODO debug

  return (StatusCode);
}

/* Verify an ECDSA Signature with STSAFE */
int stsafe_AuthSign_verify(uint8_t *inSignatureRS,
                           uint8_t *inData,
                           size_t inDataLen)
{
  int StatusCode = STSAFEA_INVALID_PARAMETER;

  // 1. Split Signature in (R,S)
  StSafeA_LVBuffer_t OutR;
  StSafeA_LVBuffer_t OutS;
  OutR.Data = inSignatureRS;
  OutR.Length = STSAFEA_XYRS_ECDSA_SHA256_LENGTH;
  OutS.Data = inSignatureRS + STSAFEA_XYRS_ECDSA_SHA256_LENGTH;
  OutS.Length = STSAFEA_XYRS_ECDSA_SHA256_LENGTH;
  //printf("SignR: "); dump_hex_str(OutR.Data, STSAFEA_XYRS_ECDSA_SHA256_LENGTH);   // TODO debug
  //printf("SignS: "); dump_hex_str(OutS.Data, STSAFEA_XYRS_ECDSA_SHA256_LENGTH);   // TODO debug

  // 2. Hash the data
  uint8_t data_Hash[STSAFEA_SHA_256_LENGTH] = {0};
  mbedtls_sha256(inData, inDataLen, data_Hash, 0);
  StSafeA_LVBuffer_t Hash;
  Hash.Data = data_Hash;
  Hash.Length = STSAFEA_SHA_256_LENGTH;
  //printf("dataHash: "); dump_hex_str(data_Hash, STSAFEA_SHA_256_LENGTH);   // TODO debug

  // 3. Get Certificate
  mbedtls_x509_crt STSAFE_Cert;
  StatusCode = stsafe_cert_NVM_read(&STSAFE_Cert);

  // 4. Get Public Key from Certificate
  StSafeA_LVBuffer_t PubX, PubY;
  uint8_t data_PubX[STSAFEA_XYRS_ECDSA_SHA256_LENGTH] = {0};
  uint8_t data_PubY[STSAFEA_XYRS_ECDSA_SHA256_LENGTH] = {0};
  PubX.Data = data_PubX; PubX.Length = STSAFEA_XYRS_ECDSA_SHA256_LENGTH;
  PubY.Data = data_PubY; PubY.Length = STSAFEA_XYRS_ECDSA_SHA256_LENGTH;

  stsafe_cert_get_public_key(&STSAFE_Cert, data_PubX, data_PubY);
  //printf("PubK-X: "); dump_hex_str(data_PubX, STSAFEA_XYRS_ECDSA_SHA256_LENGTH);   // TODO debug
  //printf("PubK-Y: "); dump_hex_str(data_PubY, STSAFEA_XYRS_ECDSA_SHA256_LENGTH);   // TODO debug

  // 5. Verify Signature on STSAFE
  StSafeA_VerifySignatureBuffer_t Verif;
  Verif.Length = 0; Verif.SignatureValidity = 0;

  StatusCode |= StSafeA_VerifyMessageSignature(&stsafea_handle,
                                               STSAFEA_NIST_P_256,
                                               &PubX, &PubY,
                                               &OutR, &OutS,
                                               &Hash,
                                               &Verif,
                                               STSAFEA_MAC_NONE);

  // Check signature validity
  if ((StatusCode == 0) && (Verif.SignatureValidity == 0U)) {
    StatusCode = (int32_t)~0U;
  }

  return (StatusCode);
}

/* Unwrap the envelope containing the PSK */
int stsafe_PSK_unwrap(uint8_t *outPSK)
{
  int StatusCode = STSAFEA_INVALID_PARAMETER;

  StSafeA_LVBuffer_t UnwrappedEnvelope;
  UnwrappedEnvelope.Length = L2SEC_PSK_ENVELOPE_SIZE;
  UnwrappedEnvelope.Data = outPSK;

  StatusCode = StSafeA_UnwrapLocalEnvelope(&stsafea_handle,
                                           STSAFEA_KEY_SLOT_0,
                                           links_psk_wrapped,
                                           L2SEC_PSK_WRAPPED_SIZE,
                                           &UnwrappedEnvelope,
                                           STSAFEA_MAC_HOST_CMAC,
                                           STSAFEA_ENCRYPTION_RESPONSE);

  return (StatusCode);
}

/* Init STSAFE for L2SEC (pairing + PSK setting) */
int stsafe_l2sec_init(void)
{
  int StatusCode;

  StatusCode = stsafe_pairing();
  StatusCode |= stsafe_PSK_init();

  return (StatusCode);
}

