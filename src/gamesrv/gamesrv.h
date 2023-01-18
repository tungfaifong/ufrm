// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_GAMESRV_H
#define UFRM_GAMESRV_H

#include <map>
#include <vector>

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
	void SendToClient(ROLEID role_id, CSID id, CSPkgBody * body);
	void SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, SSPkgBody * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);
	void BroadcastToProxy(NODETYPE node_type, SSID id, SSPkgBody * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP);

	future<std::vector<std::unordered_map<std::string, variant_t>>> DBSelect(NODEID node_id, const std::string & tb_name, const std::vector<std::string> & column, const std::unordered_map<std::string, variant_t> & where);
	future<bool> DBInsert(NODEID node_id, const std::string & tb_name, const std::vector<std::string> & column, const std::vector<variant_t> & value);
	future<bool> DBUpdate(NODEID node_id, const std::string & tb_name, const std::unordered_map<std::string, variant_t> & value, const std::unordered_map<std::string, variant_t> & where);
	future<bool> DBDelete(NODEID node_id, const std::string & tb_name, const std::unordered_map<std::string, variant_t> & where);

private:
	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);

	void _SendToGateway(NODEID node_id, SSID id, SSGWGSPkgBody * body, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);

	void _OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);
	void _OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);
	void _OnServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);

	void _OnRecvGateway(NETID net_id, const SSPkgHead & head, const SSGWGSPkgBody & body);
	void _OnRecvGatewayRpc(NETID net_id, const SSPkgHead & head, const SSGWGSPkgBody & body);

	void _OnRecvClient(NETID net_id, const SSGWGSForwardCSPkg & pkg);

private:
	void _OnGatewayInit(NETID net_id, const SSPkgHead & head, const SSGWGSInit & body);
	void _OnGatewayHeartBeatReq(NETID net_id, const SSPkgHead & head, const SSGWGSHertBeatReq & body, SSID & id, SSGWGSPkgBody * rsp_body);

private:
	NODEID _id;
	toml::table & _config;

	std::shared_ptr<ServerUnit> _server;

	std::unordered_map<NETID, NODEID> _nid2gateway;
	std::unordered_map<NODEID, NETID> _gateways;

	std::unordered_map<ROLEID, NODEID> _roles;

	LBClient _lb_client;
	PXClient _px_client;
	DBClient _db_client;
};

#endif // UFRM_GAMESRV_H
