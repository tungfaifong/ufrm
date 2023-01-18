// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COMMONSRV_H
#define UFRM_COMMONSRV_H

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

class CommonSrv : public Unit, public std::enable_shared_from_this<CommonSrv>
{
public:
	CommonSrv(NODETYPE type, NODEID id, toml::table & config);
	virtual ~CommonSrv() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

public:
	void Send(NETID net_id, google::protobuf::Message * pkg);
	void SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, google::protobuf::Message * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);
	void BroadcastToProxy(NODETYPE node_type, SSID id, google::protobuf::Message * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP);

private:
	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);

	void _OnIServerConn(NETID net_id, IP ip, PORT port);
	void _OnIServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnIServerDisc(NETID net_id);

	void _OnIServerHandeNormal(NETID net_id, const SSPkgHead & head, const std::string & data);
	void _OnIServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const std::string & data);
	void _OnIServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const std::string & data);

private:
	NODETYPE _type;
	NODEID _id;
	toml::table & _config;

	std::shared_ptr<ServerUnit> _server;
	std::shared_ptr<ServerUnit> _iserver;

	LBClient _lb_client;
	PXClient _px_client;

	std::shared_ptr<luabridge::LuaRef> _lua_on_recv_pkg;
};

#endif // UFRM_COMMONSRV_H
