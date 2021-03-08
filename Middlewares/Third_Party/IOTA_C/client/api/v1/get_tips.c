// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>

#include "json_utils.h"
#include "get_tips.h"
#include "http_lib.h"
#include "iota_str.h"

int get_tips(iota_client_conf_t const *conf, res_tips_t *res) {
  int ret = -1;
  http_response_t http_res;
  memset(&http_res, 0, sizeof(http_response_t));
  char const *const cmd_tips = "api/v1/tips";
  http_handle_t http_handle;

  // compose restful api command
  iota_str_t *cmd = iota_str_new(conf->url);
  if (cmd == NULL) {
    printf("[%s:%d]: OOM\n", __func__, __LINE__);
    return -1;
  }

  if (iota_str_append(cmd, cmd_tips)) {
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
    byte_buf2str(http_res.body);
    // json deserialization
    ret = deser_get_tips((char const *)http_res.body->data, res);
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

res_tips_t *res_tips_new(void) {
  res_tips_t *tips = malloc(sizeof(res_tips_t));
  if (tips) {
    tips->is_error = false;
    tips->u.error = NULL;
    tips->u.tips = NULL;
    return tips;
  }
  return NULL;
}

void res_tips_free(res_tips_t *tips) {
  if (tips) {
    if (tips->is_error) {
      res_err_free(tips->u.error);
    } else {
      if (tips->u.tips) {
        utarray_free(tips->u.tips);
      }
    }
    free(tips);
  }
}

int deser_get_tips(char const *const j_str, res_tips_t *res) {
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
      utarray_new(res->u.tips, &ut_str_icd);
      if ((ret = json_string_array_to_utarray(data_obj, JSON_KEY_TIP_MSG_IDS, res->u.tips)) != 0) {
        printf("[%s:%d]: parsing %s failed\n", __func__, __LINE__, JSON_KEY_TIP_MSG_IDS);
        utarray_free(res->u.tips);
        res->u.tips = NULL;
        goto end;
      }
    }
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}


size_t get_tips_id_count(res_tips_t *tips) {
  if (tips) {
    if (!tips->is_error && tips->u.tips) {
      return utarray_len(tips->u.tips);
    }
  }
  return 0;
}

char *get_tips_id(res_tips_t *tips, size_t index) {
  if (tips) {
    if (!tips->is_error && tips->u.tips) {
      return *(char **)utarray_eltptr(tips->u.tips, index);
    }
  }
  return NULL;
}
