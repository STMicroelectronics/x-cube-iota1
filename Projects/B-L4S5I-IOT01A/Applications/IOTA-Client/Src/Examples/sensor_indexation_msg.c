/**
  ******************************************************************************
  * @file    sensor_indexation_message.c
  * @author  SRA/Central LAB
  * @brief   Send to the Tangle a message including sensor data
  *
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

/* Includes ----------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>

#include "client/api/v1/send_message.h"
#include "sensors_data.h"

#include "client/network/http_lib.h"

#include "iota_conf.h"

/* Exported functions ------------------------------------------------------- */
int send_sensor_index(void)
{
  int err = 0;
  iota_client_conf_t ctx = {.host = NODE_HOST, .port = NODE_HOST_PORT, .use_tls = NODE_USE_TLS};
  res_send_message_t res;
  memset(&res, 0, sizeof(res_send_message_t));
  char jsonData[500];
  memset(jsonData, 0, sizeof(jsonData));

  // send out sensor index
  int rc = SensorDataToJSON(jsonData, sizeof(jsonData));
  printf("%s (size=%d)\n", jsonData, strlen(jsonData));
  if (0 == rc) {
    err = send_indexation_msg(&ctx, "iota_sensor_data", jsonData, &res);
    
    if (res.is_error) {
      printf("Err response: %s\n", res.u.error->msg);
      res_err_free(res.u.error);
    }
    
    if (err) {
      printf("send sensor indexation failed\n");
    } else {
      printf("message ID: %s\n", res.u.msg_id);
    }
  } else {
    printf("sensor data JSON formatting failed\n");
  }

  return 0;
}
