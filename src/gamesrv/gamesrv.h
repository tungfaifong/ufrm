// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_GAMESRV_H
#define UFRM_GAMESRV_H

#include <map>
#include <vector>

#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "toml++/toml.h"
#include "usrv/units/server_unit.h"
#include "usrv/unit.h"

#include "db_client/db_client.h"
#include "lb_client/lb_client.h"
#include "protocol/sslcls.pb.h"
#include "px_client/px_client.h"
#include "common.h"

using namespace usrv;

class GameSrv : public Unit, public std::enable_shared_from_this<GameSrv>
{
public:
	GameSrv(NODEID id, toml::table & config);
	virtual ~GameSrv() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

public:
	void SendToClient(USERID user_id, CSID id, google::protobuf::Message * body);
	void SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, google::protobuf::Message * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);
	void BroadcastToProxy(NODETYPE node_type, SSID id, google::protobuf::Message * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP);

	future<std::vector<std::unordered_map<std::string, variant_t>>> DBSelect(NODEID node_id, const std::string & tb_name, const std::vector<std::string> & column, const std::unordered_map<std::string, variant_t> & where);
	future<std::pair<uint64_t, uint64_t>> DBInsert(NODEID node_id, const std::string & tb_name, const std::vector<std::string> & column, const std::vector<variant_t> & value);
	future<bool> DBUpdate(NODEID node_id, const std::string & tb_name, const std::unordered_map<std::string, variant_t> & value, const std::unordered_map<std::string, variant_t> & where);
	future<bool> DBDelete(NODEID node_id, const std::string & tb_name, const std::unordered_map<std::string, variant_t> & where);

private:
	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);

	void _SendToGateway(NODEID node_id, SSID id, google::protobuf::Message * body, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);

	void _OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const std::string & data);
	void _OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const std::string & data);
	void _OnServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const std::string & data);

	void _OnRecvGateway(NETID net_id, const SSPkgHead & head, const std::string & data);
	void _OnRecvGatewayRpc(NETID net_id, const SSPkgHead & head, const std::string & data);

	void _OnRecvClient(NETID net_id, const SSGWGSForwardCSPkg & pkg);

private:
	void _OnGatewayInit(NETID net_id, const SSPkgHead & head, const SSGWGSInit & body);
	void _OnGatewayHeartBeatReq(NETID net_id, const SSPkgHead & head, const SSGWGSHertBeatReq & body, SSID & id, SSGSGWHertBeatRsp * rsp_body);

private:
	NODEID _id;
	toml::table & _config;

	std::shared_ptr<ServerUnit> _server;

	std::unordered_map<NETID, NODEID> _nid2gateway;
	std::unordered_map<NODEID, NETID> _gateways;

	std::unordered_map<USERID, NODEID> _users;

	LBClient _lb_client;
	PXClient _px_client;
	DBClient _db_client;

	std::shared_ptr<luabridge::LuaRef> _lua_on_recv_pkg;
};

#endif // UFRM_GAMESRV_H
