// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <string.h>

#include "json_utils.h"
#include "get_balance.h"
#include "response_error.h"
#include "http_lib.h"
#include "iota_str.h"

res_balance_t *res_balance_new(void) {
  res_balance_t *res = malloc(sizeof(res_balance_t));
  if (res) {
    res->is_error = false;
    res->u.error = NULL;
    res->u.output_balance = NULL;
    return res;
  }
  return NULL;
}

void res_balance_free(res_balance_t *res) {
  if (res) {
    if (res->is_error) {
      res_err_free(res->u.error);
    } else {
      free(res->u.output_balance);
    }
    free(res);
  }
}

int deser_balance_info(char const *const j_str, res_balance_t *res) {
  int ret = -1;

  cJSON *json_obj = cJSON_Parse(j_str);
  if (json_obj == NULL) {
    printf("[%s:%d] OOM\n", __func__, __LINE__);
    return -1;
  }

  res_err_t *res_err = deser_error(json_obj);
  if (res_err) {
    // got an error response
    res->is_error = true;
    res->u.error = res_err;
    ret = 0;
    goto end;

  } else {

    res->u.output_balance = malloc(sizeof(get_balance_t));
    if (res->u.output_balance == NULL) {
      printf("[%s:%d] OOM\n", __func__, __LINE__);
      goto end;

    } else {
    
      cJSON *data_obj = cJSON_GetObjectItemCaseSensitive(json_obj, JSON_KEY_DATA);
      if (data_obj) {
        // gets address type
        if ((ret = json_get_uint8(data_obj, JSON_KEY_ADDR_TYPE, &res->u.output_balance->address_type)) != 0) {
          printf("[%s:%d]: gets %s json max_results failed\n", __func__, __LINE__, JSON_KEY_ADDR_TYPE);
          goto end;
        }
        
        // gets address
        if ((ret = json_get_string(data_obj, JSON_KEY_ADDR, res->u.output_balance->address, sizeof(res->u.output_balance->address))) != 0) {
          res->u.output_balance->address[sizeof(res->u.output_balance->address)-1] = '\0';
          printf("[%s:%d]: gets %s failed\n", __func__, __LINE__, JSON_KEY_ADDR);
          goto end;
        }
        
        // gets balance
        if ((ret = json_get_uint64(data_obj, JSON_KEY_BALANCE, &res->u.output_balance->balance)) != 0) {
          printf("[%s:%d]: gets %s json balance failed\n", __func__, __LINE__, JSON_KEY_BALANCE);
          goto end;
        }
      }
    }
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}

int get_balance(iota_client_conf_t const *conf, char const addr[], res_balance_t *res) {
  int ret = -1;
  char const *const cmd_balance = "api/v1/addresses/ed25519/";
  http_response_t http_res;
  memset(&http_res, 0, sizeof(http_response_t));
  http_handle_t http_handle;

  if (addr == NULL || res == NULL || conf == NULL) {
    printf("[%s:%d]: get_balance failed (null parameter)\n", __func__, __LINE__);
    return -1;
  }

  if (strlen(addr) != IOTA_ADDRESS_HEX_BYTES) {
    printf("[%s:%d]: get_balance failed (invalid addr length)\n", __func__, __LINE__);
    return -1;
  }

  // compose restful api command
  iota_str_t *cmd = iota_str_new(conf->url);
  if (cmd == NULL) {
    printf("[%s:%d] OOM\n", __func__, __LINE__);
    return -1;
  }

  if (iota_str_append(cmd, cmd_balance)) {
    printf("[%s:%d]: cmd_balance append failed\n", __func__, __LINE__);
    goto done;
  }

  if (iota_str_append(cmd, addr)) {
    printf("[%s:%d]: addr append failed\n", __func__, __LINE__);
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
    byte_buf2str(http_res.body);
    // json deserialization
    ret = deser_balance_info((char const *)http_res.body->data, res);
  }

  // http close
  if (http_close(http_handle) != HTTP_OK )
  {
    printf("[%s:%d]: Can not close HTTP connection\n", __func__, __LINE__);
    ret = -1;
  }

done:
  // cleanup command
  iota_str_destroy(cmd);
  byte_buf_free(http_res.body);

  return ret;
}
