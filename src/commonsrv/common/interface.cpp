// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "interface.h"

#include "usrv/units/lua_unit.h"
#include "usrv/unit_manager.h"

#include "protocol/pblua.h"
#include "define.h"
#include "commonsrv.h"

void SendToProxy(uint16_t node_type, NODEID node_id, uint32_t id, luabridge::LuaRef lua_ref, NODEID proxy_id, uint16_t logic_type, uint16_t msg_type, size_t rpc_id)
{
	PKG_CREATE(body, SSPkgBody);
	pblua::LuaRef2PB(body, lua_ref);
	std::dynamic_pointer_cast<CommonSrv>(UnitManager::Instance()->Get("COMMONSRV"))->SendToProxy((NODETYPE)node_type, node_id, (SSID)id, body, proxy_id, (SSPkgHead::LOGICTYPE)logic_type, (SSPkgHead::MSGTYPE)msg_type, rpc_id);
}

void BroadcastToProxy(uint16_t node_type, uint32_t id, luabridge::LuaRef lua_ref, NODEID proxy_id, uint16_t logic_type)
{
	PKG_CREATE(body, SSPkgBody);
	pblua::LuaRef2PB(body, lua_ref);
	std::dynamic_pointer_cast<CommonSrv>(UnitManager::Instance()->Get("COMMONSRV"))->BroadcastToProxy((NODETYPE)node_type, (SSID)id, body, proxy_id, (SSPkgHead::LOGICTYPE)logic_type);
}

void LuaExpose(luabridge::Namespace ns)
{
	pblua::Init();
	ns.beginNamespace("pblua")
			.addFunction("Parse", pblua::Parse)
			.addFunction("Encode", pblua::Encode)
			.addFunction("Decode", pblua::Decode)
		.endNamespace()
		.beginNamespace("net")
			.addFunction("SendToProxy", SendToProxy)
			.addFunction("BroadcastToProxy", BroadcastToProxy)
		.endNamespace();
}
