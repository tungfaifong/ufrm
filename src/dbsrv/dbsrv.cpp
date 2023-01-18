// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "dbsrv.h"

#include "magic_enum.hpp"
#include "mysql++/dbdriver.h"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"

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
	if(!_lb_client.Start())
	{
		return false;
	}
	if(!_px_client.Start())
	{
		return false;
	}
	if(!_mysql_connection.connect(_config["MySql"]["db_name"].value_or(""), _config["MySql"]["ip"].value_or(""), _config["MySql"]["username"].value_or(""), _config["MySql"]["password"].value_or("")))
	{
		LOGGER_ERROR("connect mysql error db_name:{} ip:{} username:{} password:{}", _config["MySql"]["db_name"].value_or(""), _config["MySql"]["ip"].value_or(""), _config["MySql"]["username"].value_or(""), _config["MySql"]["password"].value_or(""));
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
	TraceMsg("recv ss head", &head);
	switch (head.msg_type())
	{
	case SSPkgHead::NORMAL:
		{
			_OnServerHandeNormal(net_id, head, pkg.data());
		}
		break;
	case SSPkgHead::RPCREQ:
		{
			_OnServerHanleRpcReq(net_id, head, pkg.data());
		}
		break;
	case SSPkgHead::RPCRSP:
		{
			_OnServerHanleRpcRsp(net_id, head, pkg.data());
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

void DBSrv::_SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, google::protobuf::Message * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::LOGICTYPE logic_type /* = SSPkgHead::CPP */, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	_px_client.SendToProxy(node_type, node_id, id, body, proxy_id, logic_type, msg_type, rpc_id);
}

void DBSrv::_BroadcastToProxy(NODETYPE node_type, SSID id, google::protobuf::Message * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::LOGICTYPE logic_type /* = SSPkgHead::CPP */)
{
	_px_client.BroadcastToProxy(node_type, id, body, proxy_id, logic_type);
}

void DBSrv::_OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const std::string & data)
{
	switch (head.from_node_type())
	{
	case LBSRV:
		{
			_lb_client.OnRecv(net_id, head, data);
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id());
		break;
	}
}

void DBSrv::_OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const std::string & data)
{
	std::shared_ptr<google::protobuf::Message> message = nullptr;
	SSID id;
	switch(head.id())
	{
	case SSID_DC_DS_SELECT_REQ:
		{
			UNPACK(SSDCDSSelectReq, body, data);
			message = std::make_shared<SSDSDCSelectRsp>();
			_OnSelectReq(body, id, (SSDSDCSelectRsp*)message.get());
		}
		break;
	case SSID_DC_DS_INSERT_REQ:
		{
			UNPACK(SSDCDSInsertReq, body, data);
			message = std::make_shared<SSDSDCInsertRsp>();
			_OnInsertReq(body, id, (SSDSDCInsertRsp*)message.get());
		}
		break;
	case SSID_DC_DS_UPDATE_REQ:
		{
			UNPACK(SSDCDSUpdateReq, body, data);
			message = std::make_shared<SSDSDCUpdateRsp>();
			_OnUpdateReq(body, id, (SSDSDCUpdateRsp*)message.get());
		}
		break;
	case SSID_DC_DS_DELETE_REQ:
		{
			UNPACK(SSDCDSDeleteReq, body, data);
			message = std::make_shared<SSDSDCDeleteRsp>();
			_OnDeleteReq(body, id, (SSDSDCDeleteRsp*)message.get());
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id());
		break;
	}
	_SendToProxy(head.from_node_type(), head.from_node_id(), id, message.get(), INVALID_NODE_ID, head.logic_type(), SSPkgHead::RPCRSP, head.rpc_id());
}

void DBSrv::_OnServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const std::string & data)
{
	CoroutineMgr::Instance()->Resume(head.rpc_id(), CORORESULT::SUCCESS, data);
}

void DBSrv::_OnSelectReq(const SSDCDSSelectReq & req, SSID & id, SSDSDCSelectRsp * rsp)
{
	auto column = std::vector<std::string>();
	for(auto & c : req.column())
	{
		column.push_back(c);
	}
	auto where = std::unordered_map<std::string, variant_t>();
	for(auto & [k, v] : req.where())
	{
		ConvertPBVariant2Variant(v, where[k]);
	}
	auto ret = _Select(req.tb_name(), column, where);
	id = SSID_DS_DC_SELECT_RSP;
	for(auto & row : ret)
	{
		auto rsp_row = rsp->add_result();
		auto & pb_value = *rsp_row->mutable_value();
		for(size_t i = 0; i < ret.num_fields(); ++i)
		{
			auto key = ret.field_name(i).c_str();
			auto & value = row[key];
			auto type = value.type();
			if (type == typeid(bool) || type == typeid(mysqlpp::sql_bool) || type == typeid(mysqlpp::sql_bool_null))
			{
				if(value.is_null())
				{
					pb_value[key].set_bool_(false);
				}
				else
				{
					pb_value[key].set_bool_((bool)value);
				}
			}
			else if(type == typeid(uint32_t) || type == typeid(mysqlpp::sql_int_unsigned_null))
			{
				if(value.is_null())
				{
					pb_value[key].set_uint32_(0);
				}
				else
				{
					pb_value[key].set_uint32_((uint32_t)value);
				}
			}
			else if(type == typeid(int32_t) || type == typeid(mysqlpp::sql_int_null))
			{
				if(value.is_null())
				{
					pb_value[key].set_int32_(0);
				}
				else
				{
					pb_value[key].set_int32_((int32_t)value);
				}
			}
			else if(type == typeid(uint64_t) || type == typeid(mysqlpp::sql_bigint_unsigned_null))
			{
				if(value.is_null())
				{
					pb_value[key].set_uint64_(0);
				}
				else
				{
					pb_value[key].set_uint64_((uint64_t)value);
				}
			}
			else if(type == typeid(int64_t) || type == typeid(mysqlpp::sql_bigint_null))
			{
				if(value.is_null())
				{
					pb_value[key].set_int64_(0);
				}
				else
				{
					pb_value[key].set_int64_((int64_t)value);
				}
			}
			else if(type == typeid(float) || type == typeid(mysqlpp::sql_float_null))
			{
				if(value.is_null())
				{
					pb_value[key].set_float_(0);
				}
				else
				{
					pb_value[key].set_float_((float)value);
				}
			}
			else if(type == typeid(double) || type == typeid(mysqlpp::sql_double_null))
			{
				if(value.is_null())
				{
					pb_value[key].set_double_(0);
				}
				else
				{
					pb_value[key].set_double_((double)value);
				}
			}
			else if(type == typeid(std::string) || type == typeid(mysqlpp::sql_blob) ||
					type == typeid(mysqlpp::sql_text_null) || type == typeid(mysqlpp::sql_blob_null))
			{
				if(value.is_null())
				{
					pb_value[key].set_string_("");
				}
				else
				{
					pb_value[key].set_string_((std::string)value);
				}
			}
		}
	}
}

void DBSrv::_OnInsertReq(const SSDCDSInsertReq & req, SSID & id, SSDSDCInsertRsp * rsp)
{
	auto column = std::vector<std::string>();
	for(auto & c : req.column())
	{
		column.push_back(c);
	}
	auto value = std::vector<variant_t>();
	for(auto & pb_v : req.value())
	{
		variant_t v;
		ConvertPBVariant2Variant(pb_v, v);
		value.push_back(v);
	}
	auto ret = _Insert(req.tb_name(), column, value);
	id = SSID_DS_DC_INSERT_RSP;
	rsp->set_result(ret);
}

void DBSrv::_OnUpdateReq(const SSDCDSUpdateReq & req, SSID & id, SSDSDCUpdateRsp * rsp)
{
	auto value = std::unordered_map<std::string, variant_t>();
	for(auto & [k, v] : req.value())
	{
		ConvertPBVariant2Variant(v, value[k]);
	}
	auto where = std::unordered_map<std::string, variant_t>();
	for(auto & [k, v] : req.where())
	{
		ConvertPBVariant2Variant(v, where[k]);
	}
	auto ret = _Update(req.tb_name(), value, where);
	id = SSID_DS_DC_UPDATE_RSP;
	rsp->set_result(ret);
}

void DBSrv::_OnDeleteReq(const SSDCDSDeleteReq & req, SSID & id, SSDSDCDeleteRsp * rsp)
{
	auto where = std::unordered_map<std::string, variant_t>();
	for(auto & [k, v] : req.where())
	{
		ConvertPBVariant2Variant(v, where[k]);
	}
	auto ret = _Delete(req.tb_name(), where);
	id = SSID_DS_DC_DELETE_RSP;
	rsp->set_result(ret);
}

mysqlpp::StoreQueryResult DBSrv::_Select(const std::string & tb_name, const std::vector<std::string> & column, const std::unordered_map<std::string, variant_t> & where)
{
	auto c =  _GetVecStr(column);
	c = c == "" ? "*" : c;
	auto w = _GetMapStr(where, " and ");
	w = w == "" ? w : " where " + w;
	auto sql = fmt::format("select {} from {}{};", c, tb_name, w);
	LOGGER_TRACE("sql:{}", sql);
	try
	{
		auto query = _mysql_connection.query(sql);
		auto ret = std::vector< std::unordered_map<std::string, mysqlpp::String> >();
		return query.store();
	}
	catch(const std::exception& e)
	{
		LOGGER_ERROR("select error:{}", e.what());
		return {};
	}
}

bool DBSrv::_Insert(const std::string & tb_name, const std::vector<std::string> & column, const std::vector<variant_t> & value)
{
	auto sql = fmt::format("insert into {} ({}) values ({});", tb_name, _GetVecStr(column), _GetVecStr(value));
	LOGGER_TRACE("sql:{}", sql);
	auto query = _mysql_connection.query(sql);
	try
	{
		return query.exec();
	}
	catch(const std::exception& e)
	{
		LOGGER_ERROR("insert error:{}", e.what());
		return false;
	}
}

bool DBSrv::_Update(const std::string & tb_name, const std::unordered_map<std::string, variant_t> & value, const std::unordered_map<std::string, variant_t> & where)
{
	auto w = _GetMapStr(where, " and ");
	w = w == "" ? w : " where " + w;
	auto sql = fmt::format("update {} set {}{};", tb_name, _GetMapStr(value), w);
	LOGGER_TRACE("sql:{}", sql);
	auto query = _mysql_connection.query(sql);
	try
	{
		return query.exec();
	}
	catch(const std::exception& e)
	{
		LOGGER_ERROR("update error:{}", e.what());
		return false;
	}
}

bool DBSrv::_Delete(const std::string & tb_name, const std::unordered_map<std::string, variant_t> & where)
{
	auto w = _GetMapStr(where, " and ");
	w = w == "" ? w : " where " + w;
	auto sql = fmt::format("delete from {}{};", tb_name, w);
	LOGGER_TRACE("sql:{}", sql);
	auto query = _mysql_connection.query(sql);
	try
	{
		return query.exec();
	}
	catch(const std::exception& e)
	{
		LOGGER_ERROR("delete error:{}", e.what());
		return false;
	}
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

std::string DBSrv::_GetVecStr(const std::vector<variant_t> & vec)
{
	if(vec.empty())
	{
		return "";
	}
	std::string str = "";
	for(auto iter = vec.begin(); iter != vec.end(); ++iter)
	{
		if((VARIANTIDX)iter->index() == VARIANTIDX::STRING)
		{
			std::string tmp_str = std::get<std::string>(*iter);
			_mysql_connection.driver()->escape_string(&tmp_str, 0, 0);
			str += fmt::format("'{}'", tmp_str);
		}
		else
		{
			std::visit([&str](const auto & v) {
				str += fmt::format("{}", v);
			}, *iter);
		}

		if(std::next(iter) != vec.end())
		{
			str += ", ";
		}
	}
	return str;
}

std::string DBSrv::_GetMapStr(const std::unordered_map<std::string, variant_t> & map, std::string separator /* = ", " */)
{
	if(map.empty())
	{
		return "";
	}
	std::string str = "";
	for(auto iter = map.begin(); iter != map.end(); ++iter)
	{
		auto key = iter->first;
		if((VARIANTIDX)iter->second.index() == VARIANTIDX::STRING)
		{
			std::string tmp_str = std::get<std::string>(iter->second);
			_mysql_connection.driver()->escape_string(&tmp_str, 0, 0);
			str += fmt::format("{} = '{}'", key, tmp_str);
		}
		else
		{
			std::visit([&str, &key](const auto & v) {
				str += fmt::format("{} = {}", key, v);
			}, iter->second);
		}

		if(std::next(iter) != map.end())
		{
			str += separator;
		}
	}
	return str;
}

