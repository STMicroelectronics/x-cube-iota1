// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#ifndef __CLIENT_API_V1_RES_ERR_H__
#define __CLIENT_API_V1_RES_ERR_H__

#include <stdint.h>

#include "cJSON.h"

/** @addtogroup IOTA_C
 * @{
 */

/** @addtogroup CLIENT
 * @{
 */

/** @addtogroup API
 * @{
 */

/** @defgroup RESPONSE_ERROR Response Error
 * @{
 */

/** @defgroup RESPONSE_ERROR_EXPORTED_TYPES Exported Types
 * @{
 */

/**
 * @brief Node API error response
 *
 */
typedef struct {
  char *code;  ///< an error code from node response
  char *msg;   ///< a error message from node response
} res_err_t;

/**
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup RESPONSE_ERROR_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

void res_err_free(res_err_t *err);

res_err_t *deser_error(cJSON *j_obj);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#endif
