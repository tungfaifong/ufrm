// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_ITERFACE_H
#define UFRM_ITERFACE_H

#include "common.h"

void SendToClient(ROLEID role_id, uint32_t id, const char * data, uint16_t size);
void SendToProxy(uint16_t node_type, NODEID node_id, uint32_t id, const char * data, uint16_t size, NODEID proxy_id, uint16_t logic_type, uint16_t msg_type, size_t rpc_id);
void BroadcastToProxy(uint16_t node_type, uint32_t id, const char * data, uint16_t size, NODEID proxy_id, uint16_t logic_type);

#endif // UFRM_ITERFACE_H
