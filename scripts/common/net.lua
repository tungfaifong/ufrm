-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

-- 只有gamesrv可用
function SendToClient(role_id, id, body)
	net.SendToClient(role_id, id, body)
end

function SendToProxy(node_type, node_id, id, body, proxy_id, logic_type, msg_type, rpc_id)
	proxy_id = proxy_id or 0
	logic_type = logic_type or SSPkgHead.LOGICTYPE.CPP
	msg_type = msg_type or SSPkgHead.MSGTYPE.NORMAL
	rpc_id = rpc_id or -1
	net.SendToProxy(node_type, node_id, id, body, proxy_id, logic_type, msg_type, rpc_id)
end

function BroadcastToProxy(node_type, id, body, proxy_id, logic_type)
	proxy_id = proxy_id or 0
	logic_type = logic_type or SSPkgHead.LOGICTYPE.CPP
	net.BroadcastToProxy(node_type, id, body, proxy_id, logic_type)
end

function RpcProxy(coro_id, node_type, node_id, id, body, proxy_id)
	proxy_id = proxy_id or 0
	SendToProxy(node_type, node_id, id, body, proxy_id, SSPkgHead.LOGICTYPE.LUA, SSPkgHead.MSGTYPE.RPCREQ, coro_id)
	return coroutine.yield()
end