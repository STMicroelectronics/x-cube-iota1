// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <inttypes.h>

#include "json_utils.h"
#include "get_output.h"
#include "http_lib.h"
#include "iota_str.h"

int get_output(iota_client_conf_t const *conf, char const output_id[], res_output_t *res) {
  int ret = -1;
  http_response_t http_res;
  memset(&http_res, 0, sizeof(http_response_t));
  http_handle_t http_handle;

  if (conf == NULL || output_id == NULL || res == NULL) {
    // invalid parameters
    return -1;
  }

  if (strlen(output_id) != IOTA_OUTPUT_ID_HEX_BYTES) {
    // invalid output id length
    printf("[%s:%d]: invalid output id length: %zu\n", __func__, __LINE__, strlen(output_id));
    return -1;
  }

  // compose restful api command
  iota_str_t *cmd = iota_str_new(conf->url);
  if (cmd == NULL) {
    printf("[%s:%d]: OOM\n", __func__, __LINE__);
    return -1;
  }

  if (iota_str_append(cmd, "api/v1/outputs/")) {
    printf("[%s:%d]: cmd append failed\n", __func__, __LINE__);
    goto done;
  }

  if (iota_str_append(cmd, output_id)) {
    printf("[%s:%d]: output id append failed\n", __func__, __LINE__);
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
    ret = deser_get_output((char const *)http_res.body->data, res);
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

int deser_get_output(char const *const j_str, res_output_t *res) {
  int ret = -1;
  cJSON *json_obj = cJSON_Parse(j_str);
  if (json_obj == NULL) {
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

    cJSON *data_obj = cJSON_GetObjectItemCaseSensitive(json_obj, JSON_KEY_DATA);
    if (data_obj) {
      // message ID
      if ((ret = json_get_string(data_obj, JSON_KEY_MSG_ID, res->u.output.msg_id, sizeof(res->u.output.msg_id))) != 0) {
        printf("[%s:%d]: gets %s json string failed\n", __func__, __LINE__, JSON_KEY_MSG_ID);
        goto end;
      }
      
      // transaction ID
      if ((ret = json_get_string(data_obj, JSON_KEY_TX_ID, res->u.output.tx_id, sizeof(res->u.output.tx_id))) != 0) {
        printf("[%s:%d]: gets %s json string failed\n", __func__, __LINE__, JSON_KEY_TX_ID);
        goto end;
      }
      
      // output index
      if ((ret = json_get_uint16(data_obj, JSON_KEY_OUTPUT_IDX, &res->u.output.output_idx)) != 0) {
        printf("[%s:%d]: gets %s json uint16 failed\n", __func__, __LINE__, JSON_KEY_OUTPUT_IDX);
        goto end;
      }
      
      // is spent
      if ((ret = json_get_boolean(data_obj, JSON_KEY_IS_SPENT, &res->u.output.is_spent)) != 0) {
        printf("[%s:%d]: gets %s json bool failed\n", __func__, __LINE__, JSON_KEY_IS_SPENT);
        goto end;
      }
      
      cJSON *output_obj = cJSON_GetObjectItemCaseSensitive(data_obj, JSON_KEY_OUTPUT);
      if (output_obj) {
        // output type
        if ((ret = json_get_uint32(output_obj, JSON_KEY_TYPE, &res->u.output.output_type)) != 0) {
          printf("[%s:%d]: gets output %s failed\n", __func__, __LINE__, JSON_KEY_TYPE);
          goto end;
        }
        // amount
        if ((ret = json_get_uint64(output_obj, JSON_KEY_AMOUNT, &res->u.output.amount)) != 0) {
          printf("[%s:%d]: gets output %s failed\n", __func__, __LINE__, JSON_KEY_AMOUNT);
          goto end;
        }
        
        cJSON *addr_obj = cJSON_GetObjectItemCaseSensitive(output_obj, JSON_KEY_ADDR);
        if (addr_obj) {
          // address type
          if ((ret = json_get_uint32(addr_obj, JSON_KEY_TYPE, &res->u.output.address_type)) != 0) {
            printf("[%s:%d]: gets address %s failed\n", __func__, __LINE__, JSON_KEY_TYPE);
            goto end;
          }
          
          // address
          if ((ret = json_get_string(addr_obj, JSON_KEY_ADDR, res->u.output.addr, sizeof(res->u.output.addr))) != 0) {
            printf("[%s:%d]: gets %s string failed\n", __func__, __LINE__, JSON_KEY_ADDR);
            goto end;
          }
        }
      }
    }
  }

end:
  cJSON_Delete(json_obj);

  return ret;
}

void dump_output_response(res_output_t *res) {
  if (!res) {
    return;
  }
  if (res->is_error) {
    printf("Error: %s\n", res->u.error->msg);
  } else {
    get_output_t *output = &res->u.output;
    printf("output:[\n");
    printf("\t%s addr: %.64s\n", output->address_type ? "UNKNOW" : "ED25519", output->addr);
    printf("\tmsg id: %.64s\n", output->msg_id);
    printf("\ttx id: %.64s\n", output->tx_id);
    printf("\tamount: %" PRIu64 "\n", output->amount);
    printf("\toutput_idx: %" PRIu16 "\n", output->output_idx);
    printf("\tis spent: %s\n", output->is_spent ? "True" : "False");
    printf("]\n");
  }
}
