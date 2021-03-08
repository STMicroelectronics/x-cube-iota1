// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include "find_message.h"
#include "json_utils.h"
#include "http_lib.h"
#include "iota_str.h"

static find_msg_t *find_msg_new() {
  find_msg_t *ids = malloc(sizeof(find_msg_t));
  if (ids) {
    ids->count = 0;
    ids->max_results = 0;
    utarray_new(ids->msg_ids, &ut_str_icd);
    return ids;
  }
  return NULL;
}

static void find_msg_free(find_msg_t *ids) {
  if (ids) {
    if (ids->msg_ids) {
      utarray_free(ids->msg_ids);
    }
    free(ids);
  }
}

res_find_msg_t *res_find_msg_new(void) {
  res_find_msg_t *res = malloc(sizeof(res_find_msg_t));
  if (res) {
    res->is_error = false;
    res->u.error = NULL;
    res->u.msg_ids = NULL;
    return res;
  }
  return NULL;
}

void res_find_msg_free(res_find_msg_t *res) {
  if (res) {
    if (res->is_error) {
      res_err_free(res->u.error);
    } else {
      if (res->u.msg_ids) {
        find_msg_free(res->u.msg_ids);
      }
    }
    free(res);
  }
}

size_t res_find_msg_get_id_len(res_find_msg_t *res) {
  if (res) {
    if (res->is_error == false) {
      if (res->u.msg_ids) {
        return utarray_len(res->u.msg_ids->msg_ids);
      }
    }
  }
  return 0;
}

char *res_find_msg_get_id(res_find_msg_t *res, size_t index) {
  if (res) {
    if (index < res_find_msg_get_id_len(res)) {
      char **p = (char **)utarray_eltptr(res->u.msg_ids->msg_ids, index);
      return *p;
    }
  }
  return NULL;
}

int deser_find_message(char const *const j_str, res_find_msg_t *res) {
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
      // allocate find_msg_t after parsing json object.
      res->u.msg_ids = find_msg_new();
      if (res->u.msg_ids == NULL) {
        printf("[%s:%d]: find_msg_t object allocation failed\n", __func__, __LINE__);
        goto end;
      }
      // TODO index element?
      
      // maxResults
      if ((ret = json_get_uint32(data_obj, JSON_KEY_MAX_RESULTS, &res->u.msg_ids->max_results)) != 0) {
        printf("[%s:%d]: parsing %s failed\n", __func__, __LINE__, JSON_KEY_MAX_RESULTS);
        goto end;
      }
      
      // count
      if ((ret = json_get_uint32(data_obj, JSON_KEY_COUNT, &res->u.msg_ids->count)) != 0) {
        printf("[%s:%d]: parsing %s failed\n", __func__, __LINE__, JSON_KEY_COUNT);
        goto end;
      }
      
      // message IDs
      if ((ret = json_string_array_to_utarray(data_obj, JSON_KEY_MSG_IDS, res->u.msg_ids->msg_ids)) != 0) {
        printf("[%s:%d]: parsing %s failed\n", __func__, __LINE__, JSON_KEY_MSG_IDS);
      }
      
    } else {
      printf("[%s:%d]: JSON parsing failed\n", __func__, __LINE__);
    }
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}

int find_message_by_index(iota_client_conf_t const *conf, char index[], res_find_msg_t *res) {
  int ret = -1;
  iota_str_t *cmd = NULL;
  char index_hex[65];
  memset(index_hex, 0, sizeof(index_hex));
  http_handle_t http_handle;
  http_response_t http_res;
  memset(&http_res, 0, sizeof(http_response_t));

  if (conf == NULL || index == NULL || res == NULL) {
    // invalid parameters
    return -1;
  }
  size_t index_str_len = strlen(index);
  if (index_str_len > 32) {
    printf("[%s:%d] index string too long\n", __func__, __LINE__);
    return -1;
  }

  if (string2hex(index, (byte_t *)index_hex, sizeof(index_hex)) != 0) {
    printf("[%s:%d] convert index string to hex failed\n", __func__, __LINE__);
    return -1;
  }

  // compose restful api command
  if ((cmd = iota_str_new(conf->url)) == NULL) {
    printf("[%s:%d]: OOM\n", __func__, __LINE__);
    return -1;
  }

  if (iota_str_append(cmd, "api/v1/messages?index=")) {
    printf("[%s:%d]: cmd append failed\n", __func__, __LINE__);
    goto done;
  }

  if (iota_str_append(cmd, index_hex)) {
    printf("[%s:%d]: index append failed\n", __func__, __LINE__);
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
    ret = deser_find_message((char const *)http_res.body->data, res);
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
