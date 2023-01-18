-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

db_client = {}

db_client.Select = function(coro_id, node_id, tb_name, column, where)
	local select_req = {}
	select_req.tb_name = tb_name
	select_req.column = column
	select_req.where = {}
	for k, v in pairs(where) do
		select_req.where[k] = {}
		ConvertVariant2PBVariant(v, select_req.where[k])
	end
	local result, data = RpcProxy(coro_id, NODETYPE.DBSRV, node_id, SSID.SSID_DC_DS_SELECT_REQ, "SSDCDSSelectReq", select_req)
	if result == CORORESULT.TIMEOUT then
		logger.warn("select timeout node_id:" .. node_id .. " tb_name:" .. tb_name)
		return
	end
	local rsp = pblua.Decode("SSDSDCSelectRsp", data)
	local ret = {}
	for _, row in pairs(rsp.result) do
		local w = {}
		for k, v in pairs(row.value) do
			w[k] = ConvertPBVariant2Variant(v)
		end
		table.insert(ret, w)
	end
	return ret
end

db_client.Insert = function(coro_id, node_id, tb_name, column, value)
	local insert_req = {}
	insert_req.tb_name = tb_name
	insert_req.column = column
	insert_req.value = {}
	for k, v in pairs(value) do
		insert_req.value[k] = {}
		ConvertVariant2PBVariant(v, insert_req.value[k])
	end
	local result, data = RpcProxy(coro_id, NODETYPE.DBSRV, node_id, SSID.SSID_DC_DS_INSERT_REQ, "SSDCDSInsertReq", insert_req)
	if result == CORORESULT.TIMEOUT then
		logger.warn("insert timeout node_id:" .. node_id .. " tb_name:" .. tb_name)
		return
	end
	local rsp = pblua.Decode("SSDSDCInsertRsp", data)
	return rsp.rows, rsp.insert_id
end

db_client.Update = function(coro_id, node_id, tb_name, value, where)
	local update_req = {}
	update_req.tb_name = tb_name
	update_req.value = {}
	update_req.where = {}
	for k, v in pairs(value) do
		update_req.value[k] = {}
		ConvertVariant2PBVariant(v, update_req.value[k])
	end
	for k, v in pairs(where) do
		update_req.where[k] = {}
		ConvertVariant2PBVariant(v, update_req.where[k])
	end
	local result, data = RpcProxy(coro_id, NODETYPE.DBSRV, node_id, SSID.SSID_DC_DS_UPDATE_REQ, "SSDCDSUpdateReq", update_req)
	if result == CORORESULT.TIMEOUT then
		logger.warn("update timeout node_id:" .. node_id .. " tb_name:" .. tb_name)
		return
	end
	local rsp = pblua.Decode("SSDSDCUpdateRsp", data)
	return rsp.result
end

db_client.Delete = function(coro_id, node_id, tb_name, where)
	local delete_req = {}
	delete_req.tb_name = tb_name
	delete_req.where = {}
	for k, v in pairs(where) do
		delete_req.where[k] = {}
		ConvertVariant2PBVariant(v, delete_req.where[k])
	end
	local result, data = RpcProxy(coro_id, NODETYPE.DBSRV, node_id, SSID.SSID_DC_DS_DELETE_REQ, "SSDCDSDeleteReq", delete_req)
	if result == CORORESULT.TIMEOUT then
		logger.warn("delete timeout node_id:" .. node_id .. " tb_name:" .. tb_name)
		return
	end
	local rsp =  pblua.Decode("SSDSDCDeleteRsp", data)
	return rsp.result
end

return db_client
