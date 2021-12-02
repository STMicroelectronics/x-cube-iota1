// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <inttypes.h>

#include "client/api/json_utils.h"
#include "client/api/v1/get_output.h"
#include "client/network/http_lib.h"
#include "core/utils/iota_str.h"

int get_output(iota_client_conf_t const *conf, char const output_id[], res_output_t *res) {
  int ret = -1;
  http_context_t http_ctx;
  http_response_t http_res;
  memset(&http_res, 0, sizeof(http_response_t));
  // cmd length = "/api/v1/outputs/" + IOTA_OUTPUT_ID_HEX_STR
  char cmd_buffer[85] = {0};

  if (conf == NULL || output_id == NULL || res == NULL) {
    // invalid parameters
    return -1;
  }

  if (strlen(output_id) != IOTA_OUTPUT_ID_HEX_BYTES) {
    // invalid output id length
    printf("[%s:%d]: invalid output id length: %zu\n", __func__, __LINE__, strlen(output_id));
    return -1;
  }

  // composing API command
  snprintf(cmd_buffer, sizeof(cmd_buffer), "/api/v1/outputs/%s", output_id);

  // allocate response
  http_res.body = byte_buf_new();
  if (http_res.body == NULL) {
    printf("[%s:%d]: allocate response failed\n", __func__, __LINE__);
    goto done;
  }
  http_res.code = 0;

  // http client configuration
  http_ctx.host = conf->host;
  http_ctx.path = cmd_buffer;
  http_ctx.use_tls = conf->use_tls;
  http_ctx.port = conf->port;

  // http open
  ret = http_open(&http_ctx);
  if (ret != HTTP_OK) {
    printf("[%s:%d]: Can not open HTTP connection\n", __func__, __LINE__);
    goto done;
  }

  // send request via http client
  ret = http_read(&http_ctx,
                  &http_res,
                  "Content-Type: application/json",
                  NULL);
  if (ret < 0) {
    printf("[%s:%d]: HTTP read problem\n", __func__, __LINE__);
  } else {
    byte_buf2str(http_res.body);
    // json deserialization
    ret = deser_get_output((char const *)http_res.body->data, res);
  }

  // http close
  if (http_close(&http_ctx) != HTTP_OK )
  {
    printf("[%s:%d]: Can not close HTTP connection\n", __func__, __LINE__);
    ret = -1;
  }

done:
  // cleanup command
  byte_buf_free(http_res.body);
  return ret;
}

int deser_get_output(char const *const j_str, res_output_t *res) {
  int ret = -1;
  if (j_str == NULL || res == NULL) {
    printf("[%s:%d] invalid parameter\n", __func__, __LINE__);
    return -1;
  }

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

      // ledgerIndex
      if ((ret = json_get_uint64(data_obj, JSON_KEY_LEDGER_IDX, &res->u.output.ledger_idx)) != 0) {
        printf("[%s:%d]: gets %s json uint64 failed\n", __func__, __LINE__, JSON_KEY_LEDGER_IDX);
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
    printf("\t%s addr: %s\n", output->address_type ? "UNKNOW" : "ED25519", output->addr);
    printf("\tmsg id: %s\n", output->msg_id);
    printf("\ttx id: %s\n", output->tx_id);
    printf("\tamount: %" PRIu64 "\n", output->amount);
    printf("\toutput index: %" PRIu16 "\n", output->output_idx);
    printf("\tis spent: %s\n", output->is_spent ? "True" : "False");
    printf("\tledger index: %" PRIu64 "\n", output->ledger_idx);
    printf("]\n");
  }
}
