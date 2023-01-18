// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "dbsrv.h"

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"

#include "protocol/ss.pb.h"
#include "define.h"

DBSrv::DBSrv(NODEID id, toml::table & config) : _id(id), _config(config), 
	_lb_client(DBSRV, _id, _config["DBSrv"]["ip"].value_or(DEFAULT_IP), _config["DBSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["id"].value_or(INVALID_NODE_ID), _config["LBSrv"]["ip"].value_or(DEFAULT_IP), _config["LBSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["timeout"].value_or(0)), 
	_px_client(DBSRV, _id, _config["Proxy"]["timeout"].value_or(0), _lb_client)
{

}

bool DBSrv::Init()
{
	if(_id == INVALID_NODE_ID)
	{
		LOGGER_ERROR("dbsrv id is INVALID");
		return false;
	}

	auto server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));

	server->OnConn([self = shared_from_this()](NETID net_id, IP ip, PORT port){ self->_OnServerConn(net_id, ip, port); });
	server->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size){ self->_OnServerRecv(net_id, data, size); });
	server->OnDisc([self = shared_from_this()](NETID net_id){ self->_OnServerDisc(net_id); });

	_lb_client.Init(server,
		[self = shared_from_this()](){ return 0; },
		[self = shared_from_this()](NODETYPE node_type, NODEID node_id, SSLSLCPublish::PUBLISHTYPE publish_type, IP ip, PORT port){
			self->_px_client.OnNodePublish(node_type, node_id, publish_type, ip, port);
		});

	_px_client.Init(server);

	return true;
}

bool DBSrv::Start()
{
	server::Listen(_config["DBSrv"]["port"].value_or(DEFAULT_PORT));
	if(!_lb_client.Start())
	{
		return false;
	}
	if(!_px_client.Start())
	{
		return false;
	}
	if(!_mysql_connection.connect(_config["DBSrv"]["db_name"].value_or(""), _config["DBSrv"]["ip"].value_or(""), _config["DBSrv"]["username"].value_or(""), _config["DBSrv"]["password"].value_or("")))
	{
		LOGGER_ERROR("connect mysql error db_name:{} ip:{} username:{} password:{}", _config["DBSrv"]["db_name"].value_or(""), _config["DBSrv"]["ip"].value_or(""), _config["DBSrv"]["username"].value_or(""), _config["DBSrv"]["password"].value_or(""));
		return false;
	}
	_mysql_connection.set_option(new mysqlpp::ReconnectOption(true));
	return true;
}

bool DBSrv::Update(intvl_t interval)
{
	return false;
}

void DBSrv::Stop()
{
}

void DBSrv::Release()
{
	_px_client.Release();
	_lb_client.Release();
	Unit::Release();
}

void DBSrv::_OnServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("onconnect success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void DBSrv::_OnServerRecv(NETID net_id, char * data, uint16_t size)
{
	SSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	auto body = pkg.body();
	LOGGER_TRACE("recv msg node_type:{} node_id:{} msg_type:{} id:{} rpc_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()), SSID_Name(head.id()), head.rpc_id());
	switch (head.msg_type())
	{
	case SSPkgHead::NORMAL:
		{
			_OnServerHandeNormal(net_id, head, body);
		}
		break;
	case SSPkgHead::RPCREQ:
		{
			_OnServerHanleRpcReq(net_id, head, body);
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{} msg_type:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()));
		break;
	}
}

void DBSrv::_OnServerDisc(NETID net_id)
{
	_px_client.OnDisconnect(net_id);

	LOGGER_INFO("ondisconnect success net_id:{}", net_id);
}

void DBSrv::_SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, SSPkgBody * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	_px_client.SendToProxy(node_type, node_id, id, body, proxy_id, msg_type, rpc_id);
}

void DBSrv::_BroadcastToProxy(NODETYPE node_type, SSID id, SSPkgBody * body, NODEID proxy_id /* = INVALID_NODE_ID */)
{
	_px_client.BroadcastToProxy(node_type, id, body, proxy_id);
}

void DBSrv::_OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	switch (head.from_node_type())
	{
	case LBSRV:
		{
			_lb_client.OnRecv(net_id, head, body.lcls_body());
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id());
		break;
	}
}

void DBSrv::_OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	switch (head.from_node_type())
	{
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id());
		break;
	}
}
