// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "db_client.h"

#include "usrv/interfaces/logger_interface.h"

#include "define.h"

DBClient::DBClient(PXClient & px_client) : _px_client(px_client)
{

}

future<std::vector<std::unordered_map<std::string, variant_t>>> DBClient::Select(NODEID node_id, const std::string & tb_name, const std::vector<std::string> & column, const std::unordered_map<std::string, variant_t> & where)
{
	PKG_CREATE(body, SSPkgBody);
	auto req = body->mutable_dcds_body()->mutable_select_req();
	req->set_tb_name(tb_name);
	for(auto & c : column)
	{
		req->add_column(c);
	}
	auto & pb_where = *req->mutable_where();
	for(auto & [k, v] : where)
	{
		ConvertVariant2PBVariant(v, pb_where[k]);
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

future<bool> DBClient::Update(NODEID node_id, const std::string & tb_name, const std::unordered_map<std::string, variant_t> & value, const std::unordered_map<std::string, variant_t> & where)
{
	PKG_CREATE(body, SSPkgBody);
	auto req = body->mutable_dcds_body()->mutable_update_req();
	req->set_tb_name(tb_name);
	auto & pb_value = *req->mutable_value();
	for(auto & [k, v] : where)
	{
		ConvertVariant2PBVariant(v, pb_value[k]);
	}
	auto & pb_where = *req->mutable_where();
	for(auto & [k, v] : where)
	{
		ConvertVariant2PBVariant(v, pb_where[k]);
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

future<bool> DBClient::Delete(NODEID node_id, const std::string & tb_name, const std::unordered_map<std::string, variant_t> & where)
{
	PKG_CREATE(body, SSPkgBody);
	auto req = body->mutable_dcds_body()->mutable_delete_req();
	req->set_tb_name(tb_name);
	auto & pb_where = *req->mutable_where();
	for(auto & [k, v] : where)
	{
		ConvertVariant2PBVariant(v, pb_where[k]);
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
