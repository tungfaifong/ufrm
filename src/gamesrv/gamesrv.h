// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_GAMESRV_H
#define UFRM_GAMESRV_H

#include <map>
#include <vector>

#include "toml++/toml.h"
#include "usrv/units/server_unit.h"
#include "usrv/unit.h"

#include "lb_client/lb_client.h"
#include "protocol/sslcls.pb.h"
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

private:
	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);

	void _SendToGateway(NODEID node_id, SSID id, SSGWGSPkgBody * body, MSGTYPE msg_type = MSGT_NORMAL, size_t rpc_id = -1);

	void _OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);
	void _OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);
	void _OnServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);

	void _OnRecvGateway(NETID net_id, const SSPkgHead & head, const SSGWGSPkgBody & body);
	void _OnRecvGatewayRpc(NETID net_id, const SSPkgHead & head, const SSGWGSPkgBody & body);

	void _OnRecvClient(ROLEID role_id, const CSPkg & pkg);

private:
	void _OnGatewayInit(NETID net_id, const SSPkgHead & head, const SSGWGSInit & body);
	void _OnGatewayHeartBeatReq(NETID net_id, const SSPkgHead & head, const SSGWGSHertBeatReq & body, SSID & id, SSGWGSPkgBody * rsp_body);

private:
	NODEID _id;
	toml::table & _config;

	LBClient _lb_client;

	std::unordered_map<NETID, NODEID> _nid2gateway;
	std::unordered_map<NODEID, NETID> _gateways;
};

#endif // UFRM_GAMESRV_H
