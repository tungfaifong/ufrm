// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "db_client.h"

#include "usrv/interfaces/logger_interface.h"

#include "define.h"

DBClient::DBClient(PXClient & px_client) : _px_client(px_client)
{

}

future<std::vector<std::unordered_map<std::string, variant_t>>> DBClient::Select(NODEID node_id, const std::string & tb_name, const std::vector<std::string> & column, const std::unordered_map<std::string, variant_t> & where)
{
	SSDCDSSelectReq body;
	body.set_tb_name(tb_name);
	for(auto & c : column)
	{
		body.add_column(c);
	}
	auto & pb_where = *body.mutable_where();
	for(auto & [k, v] : where)
	{
		ConvertVariant2PBVariant(v, pb_where[k]);
	}
	auto [result, data] = co_await _px_client.RpcProxy(DBSRV, node_id, SSID_DC_DS_SELECT_REQ, &body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("select timeout");
		co_return {};
	}
	SSDSDCSelectRsp rsp_body;
	auto ret = std::vector<std::unordered_map<std::string, variant_t>>();
	for(auto & row : rsp_body.result())
	{
		std::unordered_map<std::string, variant_t> w;
		for(auto & [k, v] : row.value())
		{
			ConvertPBVariant2Variant(v, w[k]);
		}
		ret.emplace_back(std::move(w));
	}
	co_return ret;
}

future<bool> DBClient::Insert(NODEID node_id, const std::string & tb_name, const std::vector<std::string> & column, const std::vector<variant_t> & value)
{
	SSDCDSInsertReq body;
	body.set_tb_name(tb_name);
	for(auto & c : column)
	{
		body.add_column(c);
	}
	for(auto & v : value)
	{
		auto pb_v = body.add_value();
		ConvertVariant2PBVariant(v, *pb_v);
	}
	auto [result, data] = co_await _px_client.RpcProxy(DBSRV, node_id, SSID_DC_DS_INSERT_REQ, &body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("insert timeout");
		co_return false;
	}
	SSDSDCInsertRsp rsp_body;
	rsp_body.ParseFromString(data);
	co_return rsp_body.result();
}

future<bool> DBClient::Update(NODEID node_id, const std::string & tb_name, const std::unordered_map<std::string, variant_t> & value, const std::unordered_map<std::string, variant_t> & where)
{
	SSDCDSUpdateReq body;
	body.set_tb_name(tb_name);
	auto & pb_value = *body.mutable_value();
	for(auto & [k, v] : value)
	{
		ConvertVariant2PBVariant(v, pb_value[k]);
	}
	auto & pb_where = *body.mutable_where();
	for(auto & [k, v] : where)
	{
		ConvertVariant2PBVariant(v, pb_where[k]);
	}
	auto [result, data] = co_await _px_client.RpcProxy(DBSRV, node_id, SSID_DC_DS_UPDATE_REQ, &body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("update timeout");
		co_return false;
	}
	SSDSDCUpdateRsp rsp_body;
	rsp_body.ParseFromString(data);
	co_return rsp_body.result();
}

future<bool> DBClient::Delete(NODEID node_id, const std::string & tb_name, const std::unordered_map<std::string, variant_t> & where)
{
	SSDCDSDeleteReq body;
	body.set_tb_name(tb_name);
	auto & pb_where = *body.mutable_where();
	for(auto & [k, v] : where)
	{
		ConvertVariant2PBVariant(v, pb_where[k]);
	}
	auto [result, data] = co_await _px_client.RpcProxy(DBSRV, node_id, SSID_DC_DS_DELETE_REQ, &body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("update timeout");
		co_return false;
	}
	SSDSDCDeleteRsp rsp_body;
	rsp_body.ParseFromString(data);
	co_return rsp_body.result();
}
