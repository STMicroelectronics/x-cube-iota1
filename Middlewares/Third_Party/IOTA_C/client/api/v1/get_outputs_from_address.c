// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include "client/api/json_utils.h"
#include "client/api/v1/get_outputs_from_address.h"
#include "client/network/http_lib.h"
#include "core/utils/iota_str.h"

static get_outputs_address_t *outputs_new(void) {
  get_outputs_address_t *ids = malloc(sizeof(get_outputs_address_t));
  if (ids) {
    memset(ids->address, 0, sizeof(ids->address));
    ids->count = 0;
    ids->max_results = 0;
    utarray_new(ids->outputs, &ut_str_icd);
    return ids;
  }
  return NULL;
}

static void outputs_free(get_outputs_address_t *ids) {
  if (ids) {
    if (ids->outputs) {
      utarray_free(ids->outputs);
    }
    free(ids);
  }
}

res_outputs_address_t *res_outputs_address_new() {
  res_outputs_address_t *res = malloc(sizeof(res_outputs_address_t));
  if (res) {
    res->is_error = false;
    res->u.output_ids = NULL;
    return res;
  }
  return NULL;
}

void res_outputs_address_free(res_outputs_address_t *res) {
  if (res) {
    if (res->is_error) {
      res_err_free(res->u.error);
    } else {
      if (res->u.output_ids) {
        outputs_free(res->u.output_ids);
      }
    }
    free(res);
  }
}

char *res_outputs_address_output_id(res_outputs_address_t *res, size_t index) {
  if (res == NULL) {
    return NULL;
  }

  if (utarray_len(res->u.output_ids->outputs)) {
    char **p = (char **)utarray_eltptr(res->u.output_ids->outputs, index);
    return *p;
  }
  return NULL;
}

size_t res_outputs_address_output_id_count(res_outputs_address_t *res) {
  if (res == NULL) {
    return 0;
  }
  return utarray_len(res->u.output_ids->outputs);
}
int deser_outputs_from_address(char const *const j_str, res_outputs_address_t *res) {
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
      res->u.output_ids = outputs_new();
      if (res->u.output_ids == NULL) {
        // OOM
        printf("[%s:%d]: allocate output object failed\n", __func__, __LINE__);
        goto end;
      }
      ret = json_get_string(data_obj, JSON_KEY_ADDR, res->u.output_ids->address, sizeof(res->u.output_ids->address));
      if (ret != 0) {
        printf("[%s:%d]: gets %s failed\n", __func__, __LINE__, JSON_KEY_ADDR);
        goto end;
      }
      ret = json_get_uint32(data_obj, JSON_KEY_MAX_RESULTS, &res->u.output_ids->max_results);
      if (ret != 0) {
        printf("[%s:%d]: gets %s failed\n", __func__, __LINE__, JSON_KEY_MAX_RESULTS);
        goto end;
      }
      ret = json_get_uint32(data_obj, JSON_KEY_COUNT, &res->u.output_ids->count);
      if (ret != 0) {
        printf("[%s:%d]: gets %s failed\n", __func__, __LINE__, JSON_KEY_COUNT);
        goto end;
      }
      ret = json_string_array_to_utarray(data_obj, JSON_KEY_OUTPUT_IDS, res->u.output_ids->outputs);
      if (ret != 0) {
        printf("[%s:%d]: gets %s failed\n", __func__, __LINE__, JSON_KEY_OUTPUT_IDS);
        goto end;
      }
      ret = json_get_uint64(data_obj, JSON_KEY_LEDGER_IDX, &res->u.output_ids->ledger_idx);
      if (ret != 0) {
        printf("[%s:%d]: gets %s failed\n", __func__, __LINE__, JSON_KEY_LEDGER_IDX);
        goto end;
      }
      
    } else {
      // JSON format mismatched.
      printf("[%s:%d]: parsing JSON object failed\n", __func__, __LINE__);
    }
  }

end:
  cJSON_Delete(json_obj);

  return ret;
}

int get_outputs_from_address(iota_client_conf_t const *conf, bool is_bech32, char const addr[], res_outputs_address_t *res) {
  int ret = -1;
  http_context_t http_ctx;
  http_response_t http_res;
  memset(&http_res, 0, sizeof(http_response_t));

  if (conf == NULL || addr == NULL || res == NULL) {
    // invalid parameters
    return -1;
  }

  size_t addr_len = strlen(addr);
  if (addr_len != IOTA_ADDRESS_HEX_BYTES) {
    printf("[%s:%d] incorrect length of the address\n", __func__, __LINE__);
    return -1;
  }

  // compose restful api command
  char cmd_buffer[99] = {0};  // 99 = max size of api path(34) + IOTA_ADDRESS_HEX_BYTES(64) + 1
  int snprintf_ret;

  if (is_bech32) {
    snprintf_ret = snprintf(cmd_buffer, sizeof(cmd_buffer), "/api/v1/addresses/%s/outputs", addr);
  } else {
    snprintf_ret = snprintf(cmd_buffer, sizeof(cmd_buffer), "/api/v1/addresses/ed25519/%s/outputs", addr);
  }

  // check if data stored is not more than buffer length
  if (snprintf_ret > (sizeof(cmd_buffer) - 1)) {
    printf("[%s:%d]: http cmd buffer overflow\n", __func__, __LINE__);
    goto done;
  }

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
    ret = deser_outputs_from_address((char const *)http_res.body->data, res);
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
