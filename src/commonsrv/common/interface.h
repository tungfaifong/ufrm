// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_CS_ITERFACE_H
#define UFRM_CS_ITERFACE_H

#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "common.h"

void SendToProxy(uint16_t node_type, NODEID node_id, uint32_t id, const std::string & proto, luabridge::LuaRef lua_ref, NODEID proxy_id, uint16_t logic_type, uint16_t msg_type, size_t rpc_id);
void BroadcastToProxy(uint16_t node_type, uint32_t id, const std::string & proto, luabridge::LuaRef lua_ref, NODEID proxy_id, uint16_t logic_type);

void LuaExpose(luabridge::Namespace ns);

#endif // UFRM_CS_ITERFACE_H
