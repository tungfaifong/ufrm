// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "interface.h"

#include "jwt-cpp/jwt.h"

#include "usrv/interfaces/logger_interface.h"
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

std::string JWTCreate( const std::string & private_key, USERID user_id, uint32_t expires_time)
{
	auto token = jwt::create()
					.set_expires_at(SysNow() + s_t(expires_time))
					.set_payload_claim("user_id", jwt::claim(std::to_string(user_id)))
					.sign(jwt::algorithm::rs256("", private_key, "", ""));

	return token;
}

bool JWTVerify(const std::string & public_key, const std::string & token)
{
	try
	{
		auto decoded_token = jwt::decode(token);
		auto verify = jwt::verify().allow_algorithm(jwt::algorithm::rs256(public_key, "", "", ""));
		verify.verify(decoded_token);
		return true;
	}
	catch (std::exception& e)
	{
		LOGGER_ERROR("jwt verify failed reason:{}, token:{}", e.what(), token);
		return false;
	}
}

std::string JWTGetPayloadClaim(const std::string & token, const std::string & key)
{
	try
	{
		auto decoded_token = jwt::decode(token);
		return decoded_token.get_payload_claim(key).as_string();
	}
	catch (std::exception& e)
	{
		LOGGER_ERROR("jwt decode failed reason:{}, token:{}", e.what(), token);
		return "";
	}
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
		.endNamespace()
		.beginNamespace("jwt")
			.addFunction("Create", JWTCreate)
			.addFunction("Verify", JWTVerify)
			.addFunction("GetPayloadClaim", JWTGetPayloadClaim)
		.endNamespace();
}
