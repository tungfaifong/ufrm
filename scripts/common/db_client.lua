-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

db_client = {}

db_client.Select = function(coro_id, node_id, tb_name, column, where)
	local body = {}
	body.dcds_body = {}
	body.dcds_body.select_req = {}
	body.dcds_body.select_req.tb_name = tb_name
	body.dcds_body.select_req.column = column
	body.dcds_body.select_req.where = {}
	for k, v in pairs(where) do
		body.dcds_body.select_req.where[k] = {}
		ConvertVariant2PBVariant(v, body.dcds_body.select_req.where[k])
	end
	local result, rsp_body = RpcProxy(coro_id, NODETYPE.DBSRV, node_id, SSID.SSID_DC_DS_SELECT_REQ, body)
	if result == CORORESULT.TIMEOUT then
		logger.warn("select timeout node_id:" .. node_id .. " tb_name:" .. tb_name)
		return
	end
	local rsp = rsp_body.dcds_body.select_rsp
	local ret = {}
	for _, row in pairs(rsp.result) do
		local w = {}
		for k, v in pairs(row) do
			w[k] = {}
			ConvertPBVariant2Variant(v, w[k])
		end
		table.insert(ret, w)
	end
	return ret
end

db_client.Insert = function(coro_id, node_id, tb_name, column, value)
	local body = {}
	body.dcds_body = {}
	body.dcds_body.insert_req = {}
	body.dcds_body.insert_req.tb_name = tb_name
	body.dcds_body.insert_req.column = column
	body.dcds_body.insert_req.value = {}
	for k, v in pairs(value) do
		body.dcds_body.insert_req.value[k] = {}
		ConvertVariant2PBVariant(v, body.dcds_body.insert_req.value[k])
	end
	local result, rsp_body = RpcProxy(coro_id, NODETYPE.DBSRV, node_id, SSID.SSID_DC_DS_INSERT_REQ, body)
	if result == CORORESULT.TIMEOUT then
		logger.warn("insert timeout node_id:" .. node_id .. " tb_name:" .. tb_name)
		return
	end
	local rsp = rsp_body.dcds_body.insert_rsp
	return rsp.result
end

db_client.Update = function(coro_id, node_id, tb_name, value, where)
	local body = {}
	body.dcds_body = {}
	body.dcds_body.update_req = {}
	body.dcds_body.update_req.tb_name = tb_name
	body.dcds_body.update_req.value = {}
	body.dcds_body.update_req.where = {}
	for k, v in pairs(value) do
		body.dcds_body.update_req.value[k] = {}
		ConvertVariant2PBVariant(v, body.dcds_body.update_req.value[k])
	end
	for k, v in pairs(where) do
		body.dcds_body.update_req.where[k] = {}
		ConvertVariant2PBVariant(v, body.dcds_body.update_req.where[k])
	end
	local result, rsp_body = RpcProxy(coro_id, NODETYPE.DBSRV, node_id, SSID.SSID_DC_DS_UPDATE_REQ, body)
	if result == CORORESULT.TIMEOUT then
		logger.warn("update timeout node_id:" .. node_id .. " tb_name:" .. tb_name)
		return
	end
	local rsp = rsp_body.dcds_body.update_rsp
	return rsp.result
end

db_client.Delete = function(coro_id, node_id, tb_name, where)
	local body = {}
	body.dcds_body = {}
	body.dcds_body.delete_req = {}
	body.dcds_body.delete_req.tb_name = tb_name
	body.dcds_body.delete_req.where = {}
	for k, v in pairs(where) do
		body.dcds_body.delete_req.where[k] = {}
		ConvertVariant2PBVariant(v, body.dcds_body.delete_req.where[k])
	end
	local result, rsp_body = RpcProxy(coro_id, NODETYPE.DBSRV, node_id, SSID.SSID_DC_DS_DELETE_REQ, body)
	if result == CORORESULT.TIMEOUT then
		logger.warn("delete timeout node_id:" .. node_id .. " tb_name:" .. tb_name)
		return
	end
	local rsp = rsp_body.dcds_body.delete_rsp
	return rsp.result
end

return db_client
