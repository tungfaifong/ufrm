// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "interface.h"

#include "usrv/units/lua_unit.h"
#include "usrv/unit_manager.h"

#include "protocol/pblua.h"
#include "define.h"
#include "commonsrv.h"

void Send(NETID net_id, const std::string & proto, luabridge::LuaRef lua_ref)
{
	std::shared_ptr<google::protobuf::Message> message = nullptr;
	pblua::LuaRef2PB(message, proto, lua_ref);
	std::dynamic_pointer_cast<CommonSrv>(UnitManager::Instance()->Get("COMMONSRV"))->Send(net_id, message.get());
}

void SendToProxy(uint16_t node_type, NODEID node_id, uint32_t id, const std::string & proto, luabridge::LuaRef lua_ref, NODEID proxy_id, uint16_t logic_type, uint16_t msg_type, size_t rpc_id)
{
	std::shared_ptr<google::protobuf::Message> message = nullptr;
	pblua::LuaRef2PB(message, proto, lua_ref);
	std::dynamic_pointer_cast<CommonSrv>(UnitManager::Instance()->Get("COMMONSRV"))->SendToProxy((NODETYPE)node_type, node_id, (SSID)id, message.get(), proxy_id, (SSPkgHead::LOGICTYPE)logic_type, (SSPkgHead::MSGTYPE)msg_type, rpc_id);
}

void BroadcastToProxy(uint16_t node_type, uint32_t id, const std::string & proto, luabridge::LuaRef lua_ref, NODEID proxy_id, uint16_t logic_type)
{
	std::shared_ptr<google::protobuf::Message> message = nullptr;
	pblua::LuaRef2PB(message, proto, lua_ref);
	std::dynamic_pointer_cast<CommonSrv>(UnitManager::Instance()->Get("COMMONSRV"))->BroadcastToProxy((NODETYPE)node_type, (SSID)id, message.get(), proxy_id, (SSPkgHead::LOGICTYPE)logic_type);
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
			.addFunction("Send", Send)
			.addFunction("SendToProxy", SendToProxy)
			.addFunction("BroadcastToProxy", BroadcastToProxy)
		.endNamespace();
}
