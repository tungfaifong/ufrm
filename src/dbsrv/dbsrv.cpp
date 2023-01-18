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

void DBSrv::_Select(std::string tb_name, std::vector<std::string> column, std::unordered_map<std::string, std::any> where)
{
	auto c =  _GetVecStr(column);
	c = c == "" ? "*" : c;
	auto w = _GetMapStr(where, " and ");
	w = w == "" ? w : " where " + w;
	auto sql = fmt::format("select {} from {}{};", c, tb_name, w);
	auto query = _mysql_connection.query(sql);
	auto ret = query.store();
}

bool DBSrv::_Insert(std::string tb_name, std::vector<std::string> column, std::vector<std::any> value)
{
	auto sql = fmt::format("insert into {} ({}) values ({});", tb_name, _GetVecStr(column), _GetVecStr(value));
	auto query = _mysql_connection.query(sql);
	return query.exec();
}

bool DBSrv::_Update(std::string tb_name, std::unordered_map<std::string, std::any> value, std::unordered_map<std::string, std::any> where)
{
	auto w = _GetMapStr(where, " and ");
	w = w == "" ? w : " where " + w;
	auto sql = fmt::format("update {} set {}{};", tb_name, _GetMapStr(value), w);
	auto query = _mysql_connection.query(sql);
	return query.exec();
}

bool DBSrv::_Delete(std::string tb_name, std::unordered_map<std::string, std::any> where)
{
	auto w = _GetMapStr(where, " and ");
	w = w == "" ? w : " where " + w;
	auto sql = fmt::format("delete from {}{};", tb_name, w);
	auto query = _mysql_connection.query(sql);
	return query.exec();
}

std::string DBSrv::_GetVecStr(const std::vector<std::string> & vec)
{
	if(vec.empty())
	{
		return "";
	}
	std::string str = "";
	for(auto iter = vec.begin(); iter != vec.end(); ++iter)
	{
		str += *iter;
		if(std::next(iter) != vec.end())
		{
			str += ", ";
		}
	}
	return str;
}

std::string DBSrv::_GetVecStr(const std::vector<std::any> & vec)
{
	if(vec.empty())
	{
		return "";
	}
	std::string str = "";
	for(auto iter = vec.begin(); iter != vec.end(); ++iter)
	{
		if (iter->type() == typeid(int32_t))
		{
			str += fmt::format("{}", std::any_cast<int32_t>(*iter));
		}
		else if(iter->type() == typeid(uint32_t))
		{
			str += fmt::format("{}", std::any_cast<uint32_t>(*iter));
		}
		else if(iter->type() == typeid(int64_t))
		{
			str += fmt::format("{}", std::any_cast<int64_t>(*iter));
		}
		else if(iter->type() == typeid(uint64_t))
		{
			str += fmt::format("{}", std::any_cast<uint64_t>(*iter));
		}
		else if(iter->type() == typeid(bool))
		{
			str += fmt::format("{}", std::any_cast<bool>(*iter));
		}
		else if(iter->type() == typeid(std::string))
		{
			str += fmt::format("'{}'", std::any_cast<std::string>(*iter));
		}

		if(std::next(iter) != vec.end())
		{
			str += ", ";
		}
	}
	return str;
}

std::string DBSrv::_GetMapStr(const std::unordered_map<std::string, std::any> & map, std::string separator /* = ", " */)
{
	if(map.empty())
	{
		return "";
	}
	std::string str = "";
	for(auto iter = map.begin(); iter != map.end(); ++iter)
	{
		if (iter->second.type() == typeid(int32_t))
		{
			str += fmt::format("{} = {}", iter->first, std::any_cast<int32_t>(iter->second));
		}
		else if(iter->second.type() == typeid(uint32_t))
		{
			str += fmt::format("{} = {}", iter->first, std::any_cast<uint32_t>(iter->second));
		}
		else if(iter->second.type() == typeid(int64_t))
		{
			str += fmt::format("{} = {}", iter->first, std::any_cast<int64_t>(iter->second));
		}
		else if(iter->second.type() == typeid(uint64_t))
		{
			str += fmt::format("{} = {}", iter->first, std::any_cast<uint64_t>(iter->second));
		}
		else if(iter->second.type() == typeid(bool))
		{
			str += fmt::format("{} = {}", iter->first, std::any_cast<bool>(iter->second));
		}
		else if(iter->second.type() == typeid(std::string))
		{
			str += fmt::format("{} = '{}'", iter->first, std::any_cast<std::string>(iter->second));
		}
		
		if(std::next(iter) != map.end())
		{
			str += separator;
		}
	}
	return str;
}

