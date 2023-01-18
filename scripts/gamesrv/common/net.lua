-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

function SendToClient(role_id, id, body)
	net.SendToClient(role_id, id, {["body"]=body})
end

function SendToProxy(node_type, node_id, id, body, proxy_id, logic_type, msg_type, rpc_id)
	proxy_id = proxy_id or 0
	logic_type = logic_type or SSPkgHead.CPP
	msg_type = msg_type or SSPkgHead.NORMAL
	rpc_id = rpc_id or -1
	net.SendToProxy(node_type, node_id, id, {["body"]=body}, proxy_id, logic_type, msg_type, rpc_id)
end

function BroadcastToProxy(node_type, id, body, proxy_id, logic_type)
	proxy_id = proxy_id or 0
	logic_type = logic_type or SSPkgHead.CPP
	net.BroadcastToProxy(node_type, id, {["body"]=body}, proxy_id, logic_type)
end