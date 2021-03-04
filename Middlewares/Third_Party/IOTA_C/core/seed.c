// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include "seed.h"
#include "iota_crypto.h"

void random_seed(byte_t seed[]) { iota_crypto_randombytes(seed, IOTA_SEED_BYTES); }
