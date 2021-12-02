// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>

#include "client/api/json_utils.h"
#include "client/api/message_builder.h"
#include "client/api/v1/get_tips.h"
#include "client/api/v1/send_message.h"
#include "core/utils/iota_str.h"
#include "client/network/http_lib.h"

char const* const cmd_msg = "/api/v1/messages";

// create JSON object and put the JSON string in byte_buf_t that can send by http client.
int serialize_indexation(message_t* msg, byte_buf_t* buf) {
  int ret = -1;
  byte_buf_t* hex_data = NULL;
  byte_buf_t* hex_index = NULL;
  char* json_string = NULL;

  if (msg == NULL || buf == NULL) {
    printf("[%s:%d] invalid parameter\n", __func__, __LINE__);
    return -1;
  }

  cJSON* json_root = cJSON_CreateObject();
  if (!json_root) {
    printf("[%s:%d] create json root object failed\n", __func__, __LINE__);
    return -1;
  }

  /*
  { "networkId": "",
    "parentMessageIds": [
        "7dabd008324378d65e607975e9f1740aa8b2f624b9e25248370454dcd07027f3",
        "9f5066de0e3225f062e9ac8c285306f56815677fe5d1db0bbccecfc8f7f1e82c"
    ],
  */
  if (!cJSON_AddStringToObject(json_root, JSON_KEY_NET_ID, "")) {
    printf("[%s:%d] networkId object failed\n", __func__, __LINE__);
    goto end;
  } else {

    if (utarray_to_json_string_array(msg->parent_msg_ids, json_root, JSON_KEY_PARENT_IDS) != JSON_OK) {
      printf("[%s:%d] add parents failed\n", __func__, __LINE__);
      goto end;
    } else {

        /*
        "payload": {
        "type": 2,
        "index": "iota.c",
        "data": "48656c6c6f"
      },
        */
        cJSON* json_payload = cJSON_AddObjectToObject(json_root, JSON_KEY_PAYLOAD);
        if (json_payload) {
          payload_index_t* payload = (payload_index_t*)msg->payload;
          if (!cJSON_AddNumberToObject(json_payload, JSON_KEY_TYPE, 2)) {
            printf("[%s:%d] payload/type object failed\n", __func__, __LINE__);
            goto end;
          }

          if ((hex_index = byte_buf_str2hex(payload->index)) != NULL) {
            if (!cJSON_AddStringToObject(json_payload, JSON_KEY_INDEX, (char const *)hex_index->data)) {
              printf("[%s:%d] payload/index object failed\n", __func__, __LINE__);
              goto end;
            }
          } else {
            printf("[%s:%d] payload/index serialization failed\n", __func__, __LINE__);
            goto end;
          }
          
          if ((hex_data = byte_buf_str2hex(payload->data)) != NULL) {
            if (!cJSON_AddStringToObject(json_payload, JSON_KEY_DATA, (char const *)hex_data->data)) {
              printf("[%s:%d] payload/data object failed\n", __func__, __LINE__);
              goto end;
            }
          } else {
            printf("[%s:%d] payload/data serialization failed\n", __func__, __LINE__);
            goto end;
          }
          
          /*
          "nonce": "" }
          */
          if (!cJSON_AddStringToObject(json_root, JSON_KEY_NONCE, "")) {
            printf("[%s:%d] nonce object failed\n", __func__, __LINE__);
            goto end;
          }
          
          // dump json object to a string
          if ((json_string = cJSON_PrintUnformatted(json_root)) == NULL) {
            printf("[%s:%d] json string print failed\n", __func__, __LINE__);
            goto end;
          }
          
          if (byte_buf_append(buf, (byte_t*)json_string, strlen(json_string) + 1) == false) {
            printf("[%s:%d] append json to buffer failed\n", __func__, __LINE__);
            goto end;
          }
          ret = 0;
        }
    }
  }

end:
  cJSON_Delete(json_root);
  byte_buf_free(hex_data);
  byte_buf_free(hex_index);
  if (json_string) {
    free(json_string);
  }
  return ret;
}

int deser_send_message_response(char const* json_str, res_send_message_t* res) {
  int ret = -1;

  // {"data":{"messageId":"322a02c8b4e7b5090b45f967f29a773dfa1dbd0302f7b9bfa253db55316581e5"}}
  cJSON* json_obj = cJSON_Parse(json_str);
  if (json_obj == NULL) {
    return -1;
  }

  res_err_t* res_err = deser_error(json_obj);
  if (res_err) {
    // got an error response
    res->is_error = true;
    res->u.error = res_err;
    ret = 0;
    goto end;
  } else {

    cJSON* data_obj = cJSON_GetObjectItemCaseSensitive(json_obj, JSON_KEY_DATA);
    if (data_obj) {
      // message ID
      if ((ret = json_get_string(data_obj, JSON_KEY_MSG_ID, res->u.msg_id, sizeof(res->u.msg_id))) != 0) {
        printf("[%s:%d]: gets %s json string failed\n", __func__, __LINE__, JSON_KEY_MSG_ID);
        goto end;
      }
      ret = 0;
    } else {
      printf("[%s:%d]: %s not found failed\n", __func__, __LINE__, JSON_KEY_DATA);
    }
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}

int send_message(iota_client_conf_t const* const conf, message_t* msg, res_send_message_t* res) {
  int ret = -1;
  byte_buf_t* json_data = byte_buf_new();
  http_context_t http_ctx;
  http_response_t http_res;
  http_res.body = byte_buf_new();
  if (!json_data || !http_res.body) {
    printf("[%s:%d] allocate http buffer failed\n", __func__, __LINE__);
    ret = -1;
    goto end;
  }
  http_res.code = 0;

  // serialize message
  switch (msg->type) {
    case MSG_PAYLOAD_TRANSACTION:
    case MSG_PAYLOAD_MILESTONE:
      printf("[%s:%d] not supported, use send_core_message instead\n", __func__, __LINE__);
      break;
    case MSG_PAYLOAD_INDEXATION:
      ret = serialize_indexation(msg, json_data);
      break;
    default:
      printf("[%s:%d] UNKNOWN message payload type\n", __func__, __LINE__);
      break;
  }

  if (ret != 0) {
    goto end;
  }

  ret = -1;
//  printf("[%s:%d]: json_data=%s\n", __func__, __LINE__, json_data->data);
  // http client configuration
  http_ctx.host = conf->host;
  http_ctx.path = cmd_msg;
  http_ctx.port = conf->port;
  http_ctx.use_tls = conf->use_tls;

  // http open
  //printf("[%s:%d]: Opening HTTP session...\n", __func__, __LINE__);
  ret = http_open(&http_ctx);
  if (ret != HTTP_OK) {
    printf("[%s:%d]: Can not open HTTP connection\n", __func__, __LINE__);
    goto end;
  }

  // send request via http client
  //printf("[%s:%d]: Sending HTTP request...\n", __func__, __LINE__);
  ret = http_read(&http_ctx,
                  &http_res,
                  "Content-Type: application/json",
                  json_data);
  if ( ret < 0 ) {
    printf("[%s:%d]: HTTP send/recv problem\n", __func__, __LINE__);
  } else {
    byte_buf2str(http_res.body);
    // deserialize node response
    ret = deser_send_message_response((char const *)http_res.body->data, res);
  }

  // http close
  //printf("[%s:%d]: Closing HTTP session...\n", __func__, __LINE__);
  if (http_close(&http_ctx) != HTTP_OK )
  {
    printf("[%s:%d]: Can not close HTTP connection\n", __func__, __LINE__);
    ret = -1;
  }

end:
  byte_buf_free(json_data);
  byte_buf_free(http_res.body);
  return ret;
}

int send_indexation_msg(iota_client_conf_t const* const conf, char const index[], char const data[],
                        res_send_message_t* res) {
  int ret = -1;
  // get tips
  res_tips_t* tips = NULL;
  message_t* msg = NULL;
  payload_index_t* idx = NULL;

  if ((tips = res_tips_new()) == NULL) {
    printf("[%s:%d] allocate tips response failed\n", __func__, __LINE__);
    return ret;
  }

  if ((ret = get_tips(conf, tips)) != 0) {
    printf("[%s:%d] get tips message failed\n", __func__, __LINE__);
    goto done;
  }

  if (tips->is_error) {
    printf("[%s:%d] get_tips response error: %s\n", __func__, __LINE__, tips->u.error->msg);
    goto done;
  }

  if ((idx = payload_index_new()) == NULL) {
    printf("[%s:%d] allocate indexation payload failed\n", __func__, __LINE__);
    goto done;
  }

  // add data and index to indexation payload
  if (!byte_buf_append(idx->data, (byte_t const*)data, strlen(data) + 1) ||
      !byte_buf_append(idx->index, (byte_t const*)index, strlen(index) + 1)) {
    printf("[%s:%d] append data and index to payload failed\n", __func__, __LINE__);
    payload_index_free(idx);
    goto done;
  }

  if ((msg = api_message_new()) == NULL) {
    printf("[%s:%d] allocate message failed\n", __func__, __LINE__);
    goto done;
  }

  // this is an indexation payload
  msg->type = MSG_PAYLOAD_INDEXATION;
  msg->payload = idx;
  utarray_concat(msg->parent_msg_ids, tips->u.tips);

  // send message to a node
  if ((ret = send_message(conf, msg, res)) != 0) {
    printf("[%s:%d] send message problem\n", __func__, __LINE__);
  }

done:
  res_tips_free(tips);
  api_message_free(msg);

  return ret;
}

int send_core_message(iota_client_conf_t const* const conf, core_message_t* msg, res_send_message_t* res) {
  int ret = -1;
  byte_buf_t* json_data = byte_buf_new();
  http_response_t http_res;
  http_context_t http_ctx;
  http_res.body = byte_buf_new();
  http_res.code = 0;
  res_tips_t* tips = NULL;
  byte_t tmp_msg_parent[IOTA_MESSAGE_ID_BYTES];
  memset(&tmp_msg_parent, 0, sizeof(tmp_msg_parent));

  if (!json_data || !http_res.body) {
    printf("[%s:%d] allocate http buffer failed\n", __func__, __LINE__);
    goto end;
  } else {
    if ((tips = res_tips_new()) == NULL) {
      printf("[%s:%d] allocate tips response failed\n", __func__, __LINE__);
      goto end;
    } else {
      // get tips
      if ((ret = get_tips(conf, tips)) != 0) {
        printf("[%s:%d] get tips failed\n", __func__, __LINE__);
        goto end;
      } else {
        if (tips->is_error) {
          printf("[%s:%d] get tips failed: %s\n", __func__, __LINE__, tips->u.error->msg);
          goto end;
        } else {
          char** p = NULL;
          p = (char**)utarray_next(tips->u.tips, p);
          while (p != NULL) {
            hex_2_bin(*p, STR_TIP_MSG_ID_LEN, tmp_msg_parent, sizeof(tmp_msg_parent));
            utarray_push_back(msg->parents, tmp_msg_parent);
            p = (char**)utarray_next(tips->u.tips, p);
          }
          char* msg_str = message_to_json(msg);
          if (!msg_str) {
            printf("[%s:%d] build message failed\n", __func__, __LINE__);
            goto end;
          }
          // put json string into byte_buf_t
          json_data->data = (byte_t*)msg_str;
          json_data->cap = json_data->len = strlen(msg_str) + 1;
        }
      }
    }
  }

  ret = -1;

  // config http client
  http_ctx.host = conf->host;
  http_ctx.path = cmd_msg;
  http_ctx.port = conf->port;
  http_ctx.use_tls = conf->use_tls;

  // http open
  ret = http_open(&http_ctx);
  if (ret != HTTP_OK) {
    printf("[%s:%d]: Can not open HTTP connection\n", __func__, __LINE__);
    goto end;
  }

  // send request via http client
  ret = http_read(&http_ctx,
                  &http_res,
                  "Content-Type: application/json",
                  json_data);
  if (ret < 0) {
    printf("[%s:%d]: HTTP read problem\n", __func__, __LINE__);
  } else {
    byte_buf2str(http_res.body);
    // deserialize node response
    ret = deser_send_message_response((char const *)http_res.body->data, res);
  }

  // http close
  if (http_close(&http_ctx) != HTTP_OK )
  {
    printf("[%s:%d]: Can not close HTTP connection\n", __func__, __LINE__);
    ret = -1;
  }

end:
  byte_buf_free(json_data);
  byte_buf_free(http_res.body);
  res_tips_free(tips);
  return ret;
}
