// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <stdlib.h>

#include "get_health.h"
#include "http_lib.h"
#include "iota_str.h"

int get_health(iota_client_conf_t const *conf, bool *health) {
  int ret = -1;
  char const *const cmd_info = "health";
  http_response_t http_res;
  memset(&http_res, 0, sizeof(http_response_t));
  http_handle_t http_handle;

  // compose restful api command
  iota_str_t *cmd = iota_str_new(conf->url);
  if (cmd == NULL) {
    printf("[%s:%d]: OOM\n", __func__, __LINE__);
    return -1;
  }

  if (iota_str_append(cmd, cmd_info)) {
    printf("[%s:%d]: string append failed\n", __func__, __LINE__);
    goto done;
  }

  // http open
  if (http_open(&http_handle, cmd->buf) != HTTP_OK) {
    printf("[%s:%d]: Can not open HTTP connection\n", __func__, __LINE__);
    goto done;
  }

  http_res.body = byte_buf_new();
  if (http_res.body == NULL) {
    printf("[%s:%d]: OOM\n", __func__, __LINE__);
    ret = -1;
    goto done;
  }
  http_res.code = 0;

  // send request via http client
  if ( http_read(http_handle,
                 &http_res,
                 "Content-Type: application/json",
                 NULL) < 0 ) {

    printf("[%s:%d]: HTTP read problem\n", __func__, __LINE__);
  } else {
    ret = 0;
  }
  
  // http close
  if (http_close(http_handle) != HTTP_OK )
  {
    printf("[%s:%d]: Can not close HTTP connection\n", __func__, __LINE__);
    ret = -1;
  }

  if (http_res.code == 200 && ret == 0) {
    *health = true;
  } else {
    *health = false;
  }

done:
  // cleanup command
  iota_str_destroy(cmd);
  byte_buf_free(http_res.body);

  return ret;
}
