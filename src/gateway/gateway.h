// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_GATEWAY_H
#define UFRM_GATEWAY_H

#include <map>
#include <vector>

#include "toml++/toml.h"
#include "usrv/units/server_unit.h"
#include "usrv/unit.h"

#include "lb_client/lb_client.h"
#include "protocol/cs.pb.h"
#include "protocol/sslcls.pb.h"
#include "px_client/px_client.h"
#include "common.h"

using namespace usrv;

class Gateway : public Unit, public std::enable_shared_from_this<Gateway>
{
public:
	static constexpr intvl_t HEART_BEAT_INTERVAL = 30000;

	struct User
	{
		NETID net_id;
		USERID user_id;
		NODEID game_id;
	};

	Gateway(NODEID id, toml::table & config);
	virtual ~Gateway() = default;

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

	void _SendToGameSrv(NODEID node_id, SSID id, google::protobuf::Message * body, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);
	awaitable_func _RpcGameSrv(NODEID node_id, SSID id, google::protobuf::Message * body);
	void _SendToClient(USERID user_id, const CSPkg & pkg);
	void _SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, google::protobuf::Message * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);
	void _BroadcastToProxy(NODETYPE node_type, SSID id, google::protobuf::Message * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP);

	void _OnIServerHandeNormal(NETID net_id, const SSPkgHead & head, const std::string & data);
	void _OnIServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const std::string & data);

	void _OnRecvGameSrv(NETID net_id, const SSPkgHead & head, const std::string & data);

private:
	future<> _ConnectToGameSrvs();
	void _OnNodePublish(NODETYPE node_type, NODEID node_id, SSLSLCPublish::PUBLISHTYPE publish_type, IP ip, PORT port);

	void _ConnectToGameSrv(NODEID node_id, IP ip, PORT port);
	void _DisconnectToGameSrv(NODEID node_id);

	void _HeartBeat();
	future<> _CoroHeartBeat(NODEID node_id);

	future<> _OnAuth(NETID net_id, USERID user_id, std::string token);
	void _ForwardToGameSrv(NETID net_id, const CSPkg & pkg);

private:
	NODEID _id;
	toml::table & _config;

	std::shared_ptr<ServerUnit> _server;
	std::shared_ptr<ServerUnit> _iserver;

	std::unordered_map<NETID, NODEID> _nid2gamesrv;
	std::unordered_map<NODEID, NETID> _gamesrvs;

	TIMERID _timer_heart_beat {INVALID_TIMER_ID};

	std::unordered_map<NETID, USERID> _nid2user;
	std::unordered_map<USERID, User> _users;

	LBClient _lb_client;
	PXClient _px_client;
};

#endif // UFRM_GATEWAY_H
