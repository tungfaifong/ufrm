// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_IOSRV_H
#define UFRM_IOSRV_H

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

class IOSrv : public Unit, public std::enable_shared_from_this<IOSrv>
{
public:
	IOSrv(NODEID id, toml::table & config);
	virtual ~IOSrv() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

private:
	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);
	void _OnIServerConn(NETID net_id, IP ip, PORT port);
	void _OnIServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnIServerDisc(NETID net_id);

	void _SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, SSPkgBody * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);
	void _BroadcastToProxy(NODETYPE node_type, SSID id, SSPkgBody * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP);

	void _OnIServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);
	void _OnIServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);
	void _OnIServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);

private:
	NODEID _id;
	toml::table & _config;

	std::shared_ptr<ServerUnit> _server;
	std::shared_ptr<ServerUnit> _iserver;

	LBClient _lb_client;
	PXClient _px_client;
	DBClient _db_client;
};

#endif // UFRM_IOSRV_H
