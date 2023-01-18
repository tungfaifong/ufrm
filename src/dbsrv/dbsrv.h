// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_DBSRV_H
#define UFRM_DBSRV_H

#include "mysql++/mysql++.h"
#include "toml++/toml.h"
#include "usrv/unit.h"

#include "lb_client/lb_client.h"
#include "protocol/ss.pb.h"
#include "px_client/px_client.h"
#include "common.h"

using namespace usrv;

class DBSrv : public Unit, public std::enable_shared_from_this<DBSrv>
{
public:
	DBSrv(NODEID id, toml::table & config);
	virtual ~DBSrv() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

private:
	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);

	void _SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, SSPkgBody * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);
	void _BroadcastToProxy(NODETYPE node_type, SSID id, SSPkgBody * body, NODEID proxy_id = INVALID_NODE_ID);

	void _OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);
	void _OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);

private:
	

private:
	NODEID _id;
	toml::table & _config;

	LBClient _lb_client;
	PXClient _px_client;

	mysqlpp::Connection _mysql_connection;
};

#endif // UFRM_DBSRV_H
