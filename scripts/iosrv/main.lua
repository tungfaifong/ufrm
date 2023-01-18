-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

package.path = package.path .. ';./scripts/?.lua;./scripts/iosrv/?.lua'

require "common.base_class"
require "common.define"
require "common.util"
require "common.coroutine_mgr"
require "common.db_client"
require "common.net"

require "common.io_net"
require "auth"
require "register"

local IO_NORMAL_HANDLER = {}
local RPC_REQ_HANDLER = {}


function Start()
	PBParse()
	
	CoroutineMgr:Instance():Start()

	IO_NORMAL_HANDLER = {
		[IOID.IOID_REGISTER_REQ] = OnRegister,
		[IOID.IOID_AUTH_REQ] = OnIOAuth,
	}

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

function OnRecv(net_id, data, size)
	local pkg = pblua.Decode("IOPkg", data)
	logger.debug(Serialize(pkg))
	local head = pkg.head
	if not IO_NORMAL_HANDLER[head.id] then
		return
	end
	IO_NORMAL_HANDLER[head.id](net_id, pkg.data)
end

function OnRecvPkg(net_id, pkg)
	local head = pkg.head
	if head.msg_type == SSPkgHead.MSGTYPE.NORMAL then
		OnIServerHandleNormal(net_id, head, pkg.data)
	elseif head.msg_type == SSPkgHead.MSGTYPE.RPCREQ then
		OnIServerHandleRpcReq(net_id, head, pkg.data)
	elseif head.msg_type == SSPkgHead.MSGTYPE.RPCRSP then
		OnIserverHandleRpcRsp(net_id, head, pkg.data)
	end
end

function OnDisc(net_id)
end

function OnIServerHandleNormal(net_id, head, data)
end

function OnIServerHandleRpcReq(net_id, head, data)
	if not RPC_REQ_HANDLER[head.id] then
		return
	end
	local id, logic_type, proto, rsp_body = RPC_REQ_HANDLER[head.id](data)
	SendToProxy(head.from_node_type, head.from_node_id, id, proto, rsp_body, 0, logic_type, SSPkgHead.MSGTYPE.RPCRSP, head.rpc_id)
end

function OnIserverHandleRpcRsp(net_id, head, data)
	CoroutineMgr:Instance():Resume(head.rpc_id, CORORESULT.SUCCESS, data)
end
