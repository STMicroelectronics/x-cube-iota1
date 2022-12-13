// Copyright 2021 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

/* Includes ------------------------------------------------------------------*/
#include <inttypes.h>
#include <stdio.h>

#include "client/network/http.h"
#include "client/api/v1/get_message.h"
#include "client/api/v1/send_message.h"

#include "sensors.h"
#include "iota_conf.h"


#define SENS_MSG_SIZE     (300)
#define SENS_MSG_DATA     "STMicroelectronics - Sensor data to Tangle:"

/* Exported functions --------------------------------------------------------*/

int send_data_message(void)
{

  iota_client_conf_t ctx = {.host = NODE_HOST, .port = NODE_PORT, .use_tls = NODE_USE_TLS};
  res_send_message_t res;

  char message[SENS_MSG_SIZE];
  snprintf(message, SENS_MSG_SIZE, "%s -- ", SENS_MSG_DATA);

  /* get sensor data from the board */
  if(sensor_data_to_json(message + 4 + strlen(SENS_MSG_DATA), SENS_MSG_SIZE) != 0) {
    printf("Sensor data JSON formatting failed\n");
    return(-1);
  }

  // send sensors data to the Tangle
  printf("Sending data message to the Tangle...\n");
  memset(&res, 0, sizeof(res_send_message_t));

  int ret = send_indexation_msg(&ctx, "iota.c\xF0\x9F\xA6\x8B", message, &res);
  if (ret == 0) {
	if (!res.is_error) {
	  printf("message id: %s\n", res.u.msg_id);
	} else {
	  printf("Node response: %s\n", res.u.error->msg);
	  res_err_free(res.u.error);
	}
	printf("\n\n");
  } else {
	printf("send_indexation_msg API failed\n");
  }


  return 0;
}
