// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "gateway.h"

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "coroutine/coroutine_mgr.h"
#include "protocol/ss.pb.h"
#include "define.h"

Gateway::Gateway(NODEID id, toml::table & config) : _id(id), _config(config), 
	_lb_client(GATEWAY, _id, _config["Gateway"]["ip"].value_or(DEFAULT_IP), _config["Gateway"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["id"].value_or(INVALID_NODE_ID), _config["LBSrv"]["ip"].value_or(DEFAULT_IP), _config["LBSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["timeout"].value_or(0)), 
	_px_client(GATEWAY, _id, _config["Proxy"]["timeout"].value_or(0), _lb_client)
{

}

bool Gateway::Init()
{
	if(_id == INVALID_NODE_ID)
	{
		LOGGER_ERROR("gateway id is INVALID");
		return false;
	}

	_server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));
	_iserver = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("ISERVER"));

	_server->OnConn([self = shared_from_this()](NETID net_id, IP ip, PORT port){ self->_OnServerConn(net_id, ip, port); });
	_server->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size){ self->_OnServerRecv(net_id, data, size); });
	_server->OnDisc([self = shared_from_this()](NETID net_id){ self->_OnServerDisc(net_id); });
	_iserver->OnConn([self = shared_from_this()](NETID net_id, IP ip, PORT port){ self->_OnIServerConn(net_id, ip, port); });
	_iserver->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size){ self->_OnIServerRecv(net_id, data, size); });
	_iserver->OnDisc([self = shared_from_this()](NETID net_id){ self->_OnIServerDisc(net_id); });

	_lb_client.Init(_iserver,
		[self = shared_from_this()](){ return self->_users.size(); },
		[self = shared_from_this()](NODETYPE node_type, NODEID node_id, SSLSLCPublish::PUBLISHTYPE publish_type, IP ip, PORT port){
			self->_OnNodePublish(node_type, node_id, publish_type, ip, port);
			self->_px_client.OnNodePublish(node_type, node_id, publish_type, ip, port);
		});
	
	_px_client.Init(_iserver);

	return true;
}

bool Gateway::Start()
{
	_server->Listen(_config["Gateway"]["port"].value_or(DEFAULT_PORT));
	if(!CoroutineMgr::Instance()->Start())
	{
		return false;
	}
	if(!_lb_client.Start())
	{
		return false;
	}
	CO_SPAWN(_ConnectToGameSrvs());
	if(!_px_client.Start())
	{
		return false;
	}
	return true;
}

bool Gateway::Update(intvl_t interval)
{
	return false;
}

void Gateway::Stop()
{
	CoroutineMgr::Instance()->Stop();
}

void Gateway::Release()
{
	_server = nullptr;
	_iserver = nullptr;
	_px_client.Release();
	_lb_client.Release();
	Unit::Release();
}

void Gateway::_OnServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("on client connect success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void Gateway::_OnServerRecv(NETID net_id, char * data, uint16_t size)
{
	CSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	TraceMsg("recv cs head", &head);
	switch (head.id())
	{
	case CSID_AUTH_REQ:
		{
			UNPACK(CSAuthReq, body, pkg.data());
			CO_SPAWN(_OnAuth(net_id, body.user_id(), body.token()));
		}
		break;
	default:
		{
			_ForwardToGameSrv(net_id, pkg);
		}
		break;
	}
}

void Gateway::_OnServerDisc(NETID net_id)
{
	if(_nid2user.find(net_id) != _nid2user.end())
	{
		auto user = _users[_nid2user[net_id]];

		// todo by agua
		SSGWGSForwardCSPkg body;
		body.set_user_id(user.user_id);
		body.mutable_cs_pkg()->mutable_head()->set_id(CSID_LOGOUT_REQ);
		_SendToGameSrv(user.game_id, SSID_GW_GS_FORWAR_CS_PKG, &body);

		_users.erase(user.user_id);
		_nid2user.erase(net_id);
	}

	LOGGER_INFO("on client disconnect success net_id:{}", net_id);
}

void Gateway::_OnIServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("onconnect success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void Gateway::_OnIServerRecv(NETID net_id, char * data, uint16_t size)
{
	SSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	TraceMsg("recv ss head", &head);
	switch (head.msg_type())
	{
	case SSPkgHead::NORMAL:
		{
			_OnIServerHandeNormal(net_id, head, pkg.data());
		}
		break;
	case SSPkgHead::RPCRSP:
		{
			_OnIServerHanleRpcRsp(net_id, head, pkg.data());
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{} msg_type:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()));
		break;
	}
}

void Gateway::_OnIServerDisc(NETID net_id)
{
	if(_nid2gamesrv.find(net_id) != _nid2gamesrv.end())
	{
		_gamesrvs.erase(_nid2gamesrv[net_id]);
		_nid2gamesrv.erase(net_id);
	}

	_px_client.OnDisconnect(net_id);

	LOGGER_INFO("ondisconnect success net_id:{}", net_id);
}

void Gateway::_SendToGameSrv(NODEID node_id, SSID id, google::protobuf::Message * body, SSPkgHead::LOGICTYPE logic_type /* = SSPkgHead::CPP */, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	if(_gamesrvs.find(node_id) == _gamesrvs.end())
	{
		LOGGER_ERROR("gamesrv:{} is invalid", node_id);
		return;
	}
	auto net_id = _gamesrvs[node_id];
	SEND_SSPKG(_iserver, net_id, GATEWAY, _id, GAMESRV, node_id, id, msg_type, rpc_id, SSPkgHead::END, logic_type, body);
}

awaitable_func Gateway::_RpcGameSrv(NODEID node_id, SSID id, google::protobuf::Message * body)
{
	return awaitable_func([this, node_id, id, body](COROID coro_id){ _SendToGameSrv(node_id, id, body, SSPkgHead::CPP, SSPkgHead::RPCREQ, coro_id); });
}

void Gateway::_SendToClient(USERID user_id, const CSPkg & pkg)
{
	if(_users.find(user_id) == _users.end())
	{
		LOGGER_ERROR("user_id:{} is invalid", user_id);
		return;
	}
	auto user = _users[user_id];
	auto net_id = user.net_id;
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("pkg size too long, id:{} size:{}", CSID_Name(pkg.head().id()), size);
		return;
	}
	_server->Send(net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	TraceMsg("send sc head", &pkg.head());
}

void Gateway::_SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, google::protobuf::Message * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::LOGICTYPE logic_type /* = SSPkgHead::CPP */, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	_px_client.SendToProxy(node_type, node_id, id, body, proxy_id, logic_type, msg_type, rpc_id);
}

void Gateway::_BroadcastToProxy(NODETYPE node_type, SSID id, google::protobuf::Message * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::LOGICTYPE logic_type /* = SSPkgHead::CPP */)
{
	_px_client.BroadcastToProxy(node_type, id, body, proxy_id, logic_type);
}

void Gateway::_OnIServerHandeNormal(NETID net_id, const SSPkgHead & head, const std::string & data)
{
	switch (head.from_node_type())
	{
	case LBSRV:
		{
			_lb_client.OnRecv(net_id, head, data);
		}
		break;
	case GAMESRV:
		{
			_OnRecvGameSrv(net_id, head, data);
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id());
		break;
	}
}

void Gateway::_OnIServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const std::string & data)
{
	CoroutineMgr::Instance()->Resume(head.rpc_id(), CORORESULT::SUCCESS, data);
}

void Gateway::_OnRecvGameSrv(NETID net_id, const SSPkgHead & head, const std::string & data)
{
	switch (head.id())
	{
	case SSID_GS_GW_FORWAR_SC_PKG:
		{
			UNPACK(SSGWGSForwardCSPkg, body, data);
			_SendToClient(body.user_id(), body.cs_pkg());
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{} id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), SSID_Name(head.id()));
		break;
	}
}

future<> Gateway::_ConnectToGameSrvs()
{
	auto game_srvs = co_await _lb_client.GetAllNodes(GAMESRV);
	for(const auto & [node_id, game_srv] : game_srvs)
	{
		_ConnectToGameSrv(node_id, game_srv.ip, game_srv.port);
	}
	_HeartBeat();
	_lb_client.Subscribe(GAMESRV);
}

void Gateway::_OnNodePublish(NODETYPE node_type, NODEID node_id, SSLSLCPublish::PUBLISHTYPE publish_type, IP ip, PORT port)
{
	if(node_type != GAMESRV)
	{
		return;
	}
	switch(publish_type)
	{
	case SSLSLCPublish::REGISTER:
		{
			_ConnectToGameSrv(node_id, ip, port);
		}
		break;
	case SSLSLCPublish::CHANGE:
		{
			_DisconnectToGameSrv(node_id);
			_ConnectToGameSrv(node_id, ip, port);
		}
		break;
	case SSLSLCPublish::UNREGISTER:
		{
			_DisconnectToGameSrv(node_id);
		}
		break;
	default:
		break;
	}
}

void Gateway::_ConnectToGameSrv(NODEID node_id, IP ip, PORT port)
{
	auto net_id = _iserver->Connect(ip, port, _config["GameSrv"]["timeout"].value_or(0));
	if(net_id == INVALID_NET_ID)
	{
		LOGGER_WARN("Connect to GameSrv failed, srv_ip:{} srv_port:{}", ip, port);
		return;
	}
	_nid2gamesrv[net_id] = node_id;
	_gamesrvs[node_id] = net_id;

	SSGWGSInit body;
	_SendToGameSrv(node_id, SSID_GW_GS_INIT, &body);
}

void Gateway::_DisconnectToGameSrv(NODEID node_id)
{
	if(_gamesrvs.find(node_id) == _gamesrvs.end())
	{
		return;
	}
	_iserver->Disconnect(_gamesrvs[node_id]);
}

void Gateway::_HeartBeat()
{
	for(auto & [node_id, net_id] : _gamesrvs)
	{
		CO_SPAWN(_CoroHeartBeat(node_id));
	}
	_timer_heart_beat = timer::CreateTimer(HEART_BEAT_INTERVAL, [this](){ _HeartBeat(); });
}

future<> Gateway::_CoroHeartBeat(NODEID node_id)
{
	SSGWGSHertBeatReq body;
	auto [result, data] = co_await _RpcGameSrv(node_id, SSID_GW_GS_HEART_BEAT_REQ, &body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("heart beat timeout");
		co_return;
	}
}

future<> Gateway::_OnAuth(NETID net_id, USERID user_id, std::string token)
{
	auto io_node = co_await _lb_client.GetLeastLoadNode(IOSRV);

	SSGWIOAuthReq body;
	body.set_user_id(user_id);
	body.set_token(token);
	auto [result, data] = co_await _px_client.RpcProxy(IOSRV, io_node.node_id, SSID_GW_IO_AUTH_REQ, &body, INVALID_NODE_ID, SSPkgHead::LUA);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("auth timeout");
		co_return;
	}

	SSIOGWAuthRsp rsp_body;
	rsp_body.ParseFromString(data);

	if(rsp_body.result() == SSIOGWAuthRsp::SUCCESS)
	{
		_nid2user[net_id] = user_id;
		
		auto gamesrv = co_await _lb_client.GetLeastLoadNode(GAMESRV);

		_users[user_id] = User {net_id, user_id, gamesrv.node_id};

		// 通知gamesrv
		SSGWGSForwardCSPkg req;
		req.set_user_id(user_id);
		req.mutable_cs_pkg()->mutable_head()->set_id(CSID_AUTH_REQ);
		_SendToGameSrv(gamesrv.node_id, SSID_GW_GS_FORWAR_CS_PKG, &req);

		// 通知client
		CSPkg pkg;
		pkg.mutable_head()->set_id(SCID_AUTH_RSP);
		SCAuthRsp rsp;
		rsp.set_result(SCAuthRsp::SUCCESS);
		rsp.SerializeToString(pkg.mutable_data());
		_SendToClient(user_id, pkg);
	}
	else
	{
		// todo 断开连接
	}
}

void Gateway::_ForwardToGameSrv(NETID net_id, const CSPkg & pkg)
{
	if(_nid2user.find(net_id) == _nid2user.end())
	{
		LOGGER_ERROR("net_id:{} is invalid", net_id);
		return;
	}
	auto user_id = _nid2user[net_id];
	auto user = _users[user_id];
	auto game_id = user.game_id;
	if(_gamesrvs.find(game_id) == _gamesrvs.end())
	{
		LOGGER_ERROR("gamesrv:{} is invalid", game_id);
		return;
	}

	SSPkgHead::LOGICTYPE logic_type = SSPkgHead::LUA;
	if(pkg.head().id() == CSID_LOGIN_REQ || pkg.head().id() == CSID_LOGOUT_REQ)
	{
		logic_type = SSPkgHead::BOTH;
	}

	SSGWGSForwardCSPkg body;
	body.set_user_id(user_id);
	auto cs_pkg = body.mutable_cs_pkg();
	*cs_pkg = pkg;
	_SendToGameSrv(game_id, SSID_GW_GS_FORWAR_CS_PKG, &body, logic_type);
}
