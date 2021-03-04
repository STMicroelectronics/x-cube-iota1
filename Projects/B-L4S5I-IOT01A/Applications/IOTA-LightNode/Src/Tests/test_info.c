// Copyright 2020 IOTA Stiftung
// SPDX-License-Identifier: Apache-2.0


#include <stdio.h>
#include <stdint.h>

#include "test_config.h"

#include "utarray.h"
#include "get_node_info.h"
#include "get_health.h"

void test_get_info(void)
{
  iota_client_conf_t ctx = {.url = TEST_NODE_ENDPOINT, .port = TEST_NODE_PORT};
  res_node_info_t* info = res_node_info_new();
  if(info == NULL) {
    return;
  }

  int ret = get_node_info(&ctx, info);
  if(info->is_error) {
    printf("Err response: %s\n", info->u.error->msg);
  }
  if(ret) {
    printf("get node info failed\n");
  } else {
    printf("node name: %s\n", info->u.output_node_info->name);
    printf("version: %s\n", info->u.output_node_info->version);
    printf("network id: %s\n", info->u.output_node_info->network_id);
  }

  res_node_info_free(info);
}

void test_get_health(void)
{
  iota_client_conf_t ctx = {.url = TEST_NODE_ENDPOINT, .port = TEST_NODE_PORT};

  bool health = false;
  int ret = get_health(&ctx, &health);
  
  if(ret) {
    printf("get health failed\n");
  } else {
    printf("isHealthy: %s\n", health ? "true" : "false");
  }
}

void test_info(void)
{
  test_get_info();
  test_get_health();
}
