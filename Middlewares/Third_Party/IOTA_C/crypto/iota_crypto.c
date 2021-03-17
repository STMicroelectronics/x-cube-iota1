// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#ifdef CRYPTO_USE_SODIUM
#include "sodium.h"
#else
#include <stdint.h>
#include <string.h>
//#include "blake2.h"
#endif

#include "iota_crypto.h"

extern struct __RNG_HandleTypeDef hrng;
int iota_rng_raw(void *data, uint8_t *output, size_t len);

void iota_crypto_randombytes(uint8_t *const buf, const size_t len) {
  iota_rng_raw(&hrng, buf, len);
}

// get ed25519 public and private key from address
void iota_crypto_keypair(uint8_t const seed[], iota_keypair_t *keypair) {
#if CRYPTO_USE_SODIUM
  crypto_sign_seed_keypair(keypair->pub, keypair->priv, seed);
#else
#endif
}

int iota_crypto_sign(uint8_t const priv_key[], uint8_t msg[], size_t msg_len, uint8_t signature[]) {
#if CRYPTO_USE_SODIUM
  unsigned long long sign_len = ED_SIGNATURE_BYTES;
  return crypto_sign_ed25519_detached(signature, &sign_len, msg, msg_len, priv_key);
#else
  return 0;
#endif
}

int iota_crypto_hmacsha256(uint8_t const secret_key[], uint8_t msg[], size_t msg_len, uint8_t auth[]) {
#if CRYPTO_USE_SODIUM
  return crypto_auth_hmacsha256(auth, msg, msg_len, secret_key);
#else
  return 0;
#endif
}

int iota_crypto_hmacsha512(uint8_t const secret_key[], uint8_t msg[], size_t msg_len, uint8_t auth[]) {
#if CRYPTO_USE_SODIUM
  return crypto_auth_hmacsha512(auth, msg, msg_len, secret_key);
#else
  return 0;
#endif
}

int iota_blake2b_sum(uint8_t const msg[], size_t msg_len, uint8_t out[], size_t out_len) {
#if CRYPTO_USE_SODIUM
  return crypto_generichash_blake2b(out, out_len, msg, msg_len, NULL, 0);
#else
  return blake2b(out, out_len, msg, msg_len, NULL, 0);
#endif
}
