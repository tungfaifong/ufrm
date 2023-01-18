// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "db_client.h"

#include "usrv/interfaces/logger_interface.h"

#include "define.h"

DBClient::DBClient(PXClient & px_client) : _px_client(px_client)
{

}

future<std::vector<std::unordered_map<std::string, variant_t>>> DBClient::Select(NODEID node_id, std::string tb_name, std::vector<std::string> column, std::unordered_map<std::string, variant_t> where)
{
	PKG_CREATE(body, SSPkgBody);
	auto req = body->mutable_dcds_body()->mutable_select_req();
	req->set_tb_name(tb_name);
	for(auto & c : column)
	{
		req->add_column(c);
	}
	auto & w = *req->mutable_where();
	for(auto & [key, value] : where)
	{
		ConvertVariant2PBVariant(value, w[key]);
	}
	auto [result, data] = co_await _px_client.RpcProxy(DBSRV, node_id, SSID_DC_DS_SELECT_REQ, body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("select timeout");
		co_return {};
	}
	SSPkgBody rsp_body;
	rsp_body.ParseFromString(data);
	auto rsp = rsp_body.dcds_body().select_rsp();
	auto ret = std::vector<std::unordered_map<std::string, variant_t>>();
	for(auto & row : rsp.result())
	{
		std::unordered_map<std::string, variant_t> w;
		for(auto & [key, value] : row.value())
		{
			ConvertPBVariant2Variant(value, w[key]);
		}
		ret.emplace_back(std::move(w));
	}
	co_return ret;
}

future<bool> DBClient::Insert(NODEID node_id, std::string tb_name, std::vector<std::string> column, std::vector<variant_t> value)
{
	PKG_CREATE(body, SSPkgBody);
	auto req = body->mutable_dcds_body()->mutable_insert_req();
	req->set_tb_name(tb_name);
	for(auto & c : column)
	{
		req->add_column(c);
	}
	for(auto & v : value)
	{
		auto pb_v = req->add_value();
		ConvertVariant2PBVariant(v, *pb_v);
	}
	auto [result, data] = co_await _px_client.RpcProxy(DBSRV, node_id, SSID_DC_DS_INSERT_REQ, body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("insert timeout");
		co_return false;
	}
	SSPkgBody rsp_body;
	rsp_body.ParseFromString(data);
	auto rsp = rsp_body.dcds_body().insert_rsp();
	co_return rsp.result();
}

future<bool> DBClient::Update(NODEID node_id, std::string tb_name, std::unordered_map<std::string, variant_t> value, std::unordered_map<std::string, variant_t> where)
{
	PKG_CREATE(body, SSPkgBody);
	auto req = body->mutable_dcds_body()->mutable_update_req();
	req->set_tb_name(tb_name);
	auto & v = *req->mutable_value();
	for(auto & [key, value] : where)
	{
		ConvertVariant2PBVariant(value, v[key]);
	}
	auto & w = *req->mutable_where();
	for(auto & [key, value] : where)
	{
		ConvertVariant2PBVariant(value, w[key]);
	}
	auto [result, data] = co_await _px_client.RpcProxy(DBSRV, node_id, SSID_DC_DS_UPDATE_REQ, body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("update timeout");
		co_return false;
	}
	SSPkgBody rsp_body;
	rsp_body.ParseFromString(data);
	auto rsp = rsp_body.dcds_body().update_rsp();
	co_return rsp.result();
}

future<bool> DBClient::Delete(NODEID node_id, std::string tb_name, std::unordered_map<std::string, variant_t> where)
{
	PKG_CREATE(body, SSPkgBody);
	auto req = body->mutable_dcds_body()->mutable_delete_req();
	req->set_tb_name(tb_name);
	auto & w = *req->mutable_where();
	for(auto & [key, value] : where)
	{
		ConvertVariant2PBVariant(value, w[key]);
	}
	auto [result, data] = co_await _px_client.RpcProxy(DBSRV, node_id, SSID_DC_DS_DELETE_REQ, body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("update timeout");
		co_return false;
	}
	SSPkgBody rsp_body;
	rsp_body.ParseFromString(data);
	auto rsp = rsp_body.dcds_body().delete_rsp();
	co_return rsp.result();
}
