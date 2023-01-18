-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

function SendToClient(role_id, id, body)
	local data, size = pblua.encode("CSPkgBody", body)
	gamesrv.SendToClient(role_id, id, data, size)
end

function SendToProxy(node_type, node_id, id, body, proxy_id, logic_type, msg_type, rpc_id)
	local data, size = pblua.encode("SSPkgBody", body)
	gamesrv.SendToProxy(node_type, node_id, id, data, size, proxy_id, logic_type, msg_type, rpc_id)
end

function BroadcastToProxy(node_type, id, body, proxy_id, logic_type)
	local data, size = pblua.encode("SSPkgBody", body)
	gamesrv.BroadcastToProxy(node_type, id, data, size, proxy_id, logic_type)
end