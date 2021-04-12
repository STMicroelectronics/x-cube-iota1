// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#ifndef __CLIENT_API_MESSAGE_BUILDER_H__
#define __CLIENT_API_MESSAGE_BUILDER_H__

#include "models_message.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup IOTA_C
 * @{
 */

/** @addtogroup CLIENT
 * @{
 */

/** @addtogroup API
 * @{
 */

/** @defgroup MESSAGE_Builder Message builder
 * @{
 */

/** @defgroup MESSAGE_Builder_EXPORTED_FUNCTIONS Exported Functions
 * @{
 */

/**
 * @brief Serialize message object to a JSON string
 *
 * @param[in] msg A message object
 * @return char*
 */
char* message_to_json(core_message_t* msg);

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

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
