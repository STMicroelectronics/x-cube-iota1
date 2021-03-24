// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <string.h>

#include "unity.h"

#include "address.h"
#include "indexation.h"
//#include "utxo_input.h"
#include "sodium.h"

/*
static byte_t tx_id0[TRANSACTION_ID_BYTES] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static byte_t tx_id1[TRANSACTION_ID_BYTES] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                              255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                              255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
static byte_t tx_id2[TRANSACTION_ID_BYTES] = {126, 127, 95,  249, 151, 44,  243, 150, 40,  39, 46,
                                              190, 54,  49,  73,  171, 165, 88,  139, 221, 25, 199,
                                              90,  172, 252, 142, 91,  179, 113, 2,   177, 58};
*/

void test_address_gen() {
  char const* const exp_iot_bech32 = "iot1q9g4tqh7vj9s7y9zk2smj8t4qgvse9um42l7apdkhw6syp5ju4w3vqkuhey";
  char const* const exp_iota_bech32 = "iota1q9g4tqh7vj9s7y9zk2smj8t4qgvse9um42l7apdkhw6syp5ju4w3vy67kjg";
  byte_t exp_addr[IOTA_ADDRESS_BYTES] = {0x01, 0x51, 0x55, 0x82, 0xfe, 0x64, 0x8b, 0xf,  0x10, 0xa2, 0xb2,
                                         0xa1, 0xb9, 0x1d, 0x75, 0x2,  0x19, 0xc,  0x97, 0x9b, 0xaa, 0xbf,
                                         0xee, 0x85, 0xb6, 0xbb, 0xb5, 0x2,  0x6,  0x92, 0xe5, 0x5d, 0x16};
  byte_t exp_ed_addr[ED25519_ADDRESS_BYTES] = {0x4d, 0xbc, 0x7b, 0x45, 0x32, 0x46, 0x64, 0x20, 0x9a, 0xe5, 0x59,
                                               0xcf, 0xd1, 0x73, 0xc,  0xb,  0xb1, 0x90, 0x5a, 0x7f, 0x83, 0xe6,
                                               0x5d, 0x48, 0x37, 0xa9, 0x87, 0xe2, 0x24, 0xc1, 0xc5, 0x1e};

  byte_t seed[IOTA_SEED_BYTES];
  memset(seed, 0, sizeof(seed));
  byte_t addr_from_path[ED25519_ADDRESS_BYTES];
  memset(addr_from_path, 0, sizeof(addr_from_path));
  char bech32_addr[128];
  memset(bech32_addr, 0, sizeof(bech32_addr));
  byte_t addr_with_ver[IOTA_ADDRESS_BYTES];
  memset(addr_with_ver, 0, sizeof(addr_with_ver));
  byte_t addr_from_bech32[IOTA_ADDRESS_BYTES];
  memset(addr_from_bech32, 0, sizeof(addr_from_bech32));

  // convert seed from hex string to binary
  TEST_ASSERT(hex2bin("e57fb750f3a3a67969ece5bd9ae7eef5b2256a818b2aac458941f7274985a410", 64, seed, IOTA_SEED_BYTES) ==
              0);

  TEST_ASSERT(address_from_path(seed, "m/44'/4218'/0'/0'/0'", addr_from_path) == 0);
  // dump_hex(addr_from_path, ED25519_ADDRESS_BYTES);

  // ed25519 address to IOTA address
  addr_with_ver[0] = ADDRESS_VER_ED25519;
  memcpy(addr_with_ver + 1, addr_from_path, ED25519_ADDRESS_BYTES);
  TEST_ASSERT_EQUAL_MEMORY(exp_addr, addr_with_ver, IOTA_ADDRESS_BYTES);

  // convert binary address to bech32 with iot HRP
  TEST_ASSERT(address_2_bech32(addr_with_ver, "iot", bech32_addr) == 0);
  TEST_ASSERT_EQUAL_STRING(exp_iot_bech32, bech32_addr);
  printf("bech32 [iot]: %s\n", bech32_addr);
  // bech32 to binary address
  TEST_ASSERT(address_from_bech32("iot", bech32_addr, addr_from_bech32) == 0);
  TEST_ASSERT_EQUAL_MEMORY(addr_with_ver, addr_from_bech32, IOTA_ADDRESS_BYTES);

  // convert binary address to bech32 with iota HRP
  TEST_ASSERT(address_2_bech32(addr_with_ver, "iota", bech32_addr) == 0);
  TEST_ASSERT_EQUAL_STRING(exp_iota_bech32, bech32_addr);
  printf("bech32 [iota]: %s\n", bech32_addr);
  // bech32 to binary address
  TEST_ASSERT(address_from_bech32("iota", bech32_addr, addr_from_bech32) == 0);
  TEST_ASSERT_EQUAL_MEMORY(addr_with_ver, addr_from_bech32, IOTA_ADDRESS_BYTES);

  // address from ed25519 keypair
  iota_keypair_t seed_keypair;
  memset(&seed_keypair, 0, sizeof(seed_keypair));
  byte_t ed_addr[ED25519_ADDRESS_BYTES];
  memset(ed_addr, 0, sizeof(ed_addr));

  // address from ed25519 public key
  iota_crypto_keypair(seed, &seed_keypair);
  TEST_ASSERT(address_from_ed25519_pub(seed_keypair.pub, ed_addr) == 0);
  TEST_ASSERT_EQUAL_MEMORY(exp_ed_addr, ed_addr, ED25519_ADDRESS_BYTES);
  // dump_hex(ed_addr, ED25519_ADDRESS_BYTES);
}


void test_indexation() {
  char const* const exp_index = "HELLO";
  byte_t exp_data[12] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x21};
  byte_t exp_serialized[27] = {0x2, 0x0,  0x0,  0x0,  0x5,  0x0,  0x48, 0x45, 0x4C, 0x4C, 0x4F, 0xC,  0x0, 0x0,
                               0x0, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x21};

  indexation_t* idx = indexation_new();
  TEST_ASSERT_NOT_NULL(idx);
  indexation_free(idx);
  idx = NULL;

  TEST_ASSERT_NULL(idx);
  idx = indexation_create(exp_index, exp_data, sizeof(exp_data));
  TEST_ASSERT_NOT_NULL(idx);

  // validate index
  TEST_ASSERT_EQUAL_STRING(exp_index, idx->index->data);
  TEST_ASSERT((strlen(exp_index) + 1) == idx->index->len);

  // validate data
  TEST_ASSERT_EQUAL_MEMORY(exp_data, idx->data->data, sizeof(exp_data));

  // serialization
  size_t serialized_len = indexaction_serialize_length(idx);
  byte_t* serialized_data = malloc(serialized_len);
  size_t actual_len = indexation_payload_serialize(idx, serialized_data);
  TEST_ASSERT(serialized_len = actual_len);

  TEST_ASSERT_EQUAL_MEMORY(exp_serialized, serialized_data, sizeof(exp_serialized));
  dump_hex_str(serialized_data, serialized_len);

  free(serialized_data);
  indexation_free(idx);
}

/*
void test_utxo_input() {
  utxo_input_ht* inputs = utxo_inputs_new();
  TEST_ASSERT_NULL(inputs);

  TEST_ASSERT_EQUAL_UINT16(0, utxo_inputs_count(&inputs));
  TEST_ASSERT(utxo_inputs_add(&inputs, tx_id0, UINT8_MAX) == -1);
  TEST_ASSERT_EQUAL_UINT16(0, utxo_inputs_count(&inputs));
  TEST_ASSERT(utxo_inputs_add(&inputs, tx_id0, 0) == 0);
  TEST_ASSERT_EQUAL_UINT16(1, utxo_inputs_count(&inputs));

  // transaction ID doesn't exist.
  TEST_ASSERT_NULL(utxo_inputs_find_by_id(&inputs, tx_id2));

  // add more tx IDs
  TEST_ASSERT(utxo_inputs_add(&inputs, tx_id1, 1) == 0);
  TEST_ASSERT(utxo_inputs_add(&inputs, tx_id2, 2) == 0);
  TEST_ASSERT_EQUAL_UINT16(3, utxo_inputs_count(&inputs));

  // find and validate transaction ID
  utxo_input_ht* elm = utxo_inputs_find_by_id(&inputs, tx_id1);
  TEST_ASSERT_NOT_NULL(elm);
  TEST_ASSERT_EQUAL_MEMORY(tx_id1, elm->tx_id, TRANSACTION_ID_BYTES);
  TEST_ASSERT(1 == elm->output_index);
  TEST_ASSERT_EQUAL_UINT16(3, utxo_inputs_count(&inputs));

  utxo_inputs_print(&inputs);

  utxo_inputs_free(&inputs);
}
*/

int test_core(void) {
  UNITY_BEGIN();

  RUN_TEST(test_address_gen);
  RUN_TEST(test_indexation);
//  RUN_TEST(test_utxo_input);

  return UNITY_END();
}
