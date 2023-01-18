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

	void _SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, google::protobuf::Message * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);
	void _BroadcastToProxy(NODETYPE node_type, SSID id, google::protobuf::Message * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP);

	void _OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const std::string & data);
	void _OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const std::string & data);
	void _OnServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const std::string & data);

private:
	void _OnSelectReq(const SSDCDSSelectReq & req, SSID & id, SSDSDCSelectRsp * rsp);
	void _OnInsertReq(const SSDCDSInsertReq & req, SSID & id, SSDSDCInsertRsp * rsp);
	void _OnUpdateReq(const SSDCDSUpdateReq & req, SSID & id, SSDSDCUpdateRsp * rsp);
	void _OnDeleteReq(const SSDCDSDeleteReq & req, SSID & id, SSDSDCDeleteRsp * rsp);

private:
	mysqlpp::StoreQueryResult _Select(const std::string & tb_name, const std::vector<std::string> & column, const std::unordered_map<std::string, variant_t> & where);
	mysqlpp::SimpleResult _Insert(const std::string & tb_name, const std::vector<std::string> & column, const std::vector<variant_t> & value);
	bool _Update(const std::string & tb_name, const std::unordered_map<std::string, variant_t> & value, const std::unordered_map<std::string, variant_t> & where);
	bool _Delete(const std::string & tb_name, const std::unordered_map<std::string, variant_t> & where);

	std::string _GetVecStr(const std::vector<std::string> & vec);
	std::string _GetVecStr(const std::vector<variant_t> & vec);
	std::string _GetMapStr(const std::unordered_map<std::string, variant_t> & map, std::string separator = ", ");

private:
	NODEID _id;
	toml::table & _config;

	LBClient _lb_client;
	PXClient _px_client;

	mysqlpp::Connection _mysql_connection;
};

#endif // UFRM_DBSRV_H
