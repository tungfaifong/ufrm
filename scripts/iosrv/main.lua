-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

package.path = package.path .. ';./scripts/?.lua;./scripts/iosrv/?.lua'

require "common.base_class"
require "common.define"
require "common.util"
require "common.coroutine_mgr"
require "common.db_client"
require "common.net"

require "auth"

local RPC_REQ_HANDLER = {}

function Start()
	PBParse()
	
	CoroutineMgr:Instance():Start()

	RPC_REQ_HANDLER = {
		[SSID.SSID_GW_IO_AUTH_REQ] = OnAuth,
	}

	return true
end

function Update(interval)
	return false
end

function Stop()
end

function OnConn(net_id, ip, port)
	CoroutineMgr:Instance():Stop()
end

function OnRecv(net_id, data)
end

function OnRecvPkg(net_id, pkg)
	local head = pkg.head
	local body = pkg.body
	if head.msg_type == SSPkgHead.MSGTYPE.NORMAL then
		OnServerHandleNormal(net_id, head, body)
	elseif head.msg_type == SSPkgHead.MSGTYPE.RPCREQ then
		OnServerHandleRpcReq(net_id, head, body)
	elseif head.msg_type == SSPkgHead.MSGTYPE.RPCRSP then
		OnserverHandleRpcRsp(net_id, head, body)
	end
end

function OnDisc(net_id)
end

function OnServerHandleNormal(net_id, head, body)
	
end

function OnServerHandleRpcReq(net_id, head, body)
	if not RPC_REQ_HANDLER[head.id] then
		return
	end
	local id, logic_type, rsp_body = RPC_REQ_HANDLER[head.id](body)
	SendToProxy(head.from_node_type, head.from_node_id, id, rsp_body, 0, logic_type, SSPkgHead.MSGTYPE.RPCRSP, head.rpc_id)
end

function OnserverHandleRpcRsp(net_id, head, body)
	CoroutineMgr:Instance():Resume(head.rpc_id, CORORESULT.SUCCESS, body)
end
