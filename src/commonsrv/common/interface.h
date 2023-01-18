// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_CS_ITERFACE_H
#define UFRM_CS_ITERFACE_H

#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "common.h"

void Send(NETID net_id, const std::string & proto, luabridge::LuaRef lua_ref);
void SendToProxy(uint16_t node_type, NODEID node_id, uint32_t id, const std::string & proto, luabridge::LuaRef lua_ref, NODEID proxy_id, uint16_t logic_type, uint16_t msg_type, size_t rpc_id);
void BroadcastToProxy(uint16_t node_type, uint32_t id, const std::string & proto, luabridge::LuaRef lua_ref, NODEID proxy_id, uint16_t logic_type);

std::string JWTCreate( const std::string & private_key, USERID user_id, uint32_t expires_time);
bool JWTVerify(const std::string & public_key, const std::string & token);
std::string JWTGetPayloadClaim(const std::string & token, const std::string & key);

void LuaExpose(luabridge::Namespace ns);

#endif // UFRM_CS_ITERFACE_H
