-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

package.path = package.path .. ';./scripts/?.lua;./scripts/iosrv/?.lua'

require "common.base_class"
require "common.define"
require "common.util"
require "common.coroutine_mgr"
require "common.db_client"
require "common.net"

function Start()
	pblua.Parse("common.proto")
	pblua.Parse("cs.proto")
	pblua.Parse("csid.proto")
	pblua.Parse("io.proto")
	pblua.Parse("ioid.proto")
	pblua.Parse("ss.proto")
	pblua.Parse("ssdcds.proto")
	pblua.Parse("ssgwgs.proto")
	pblua.Parse("ssid.proto")
	pblua.Parse("sslcls.proto")
	pblua.Parse("sspcpx.proto")

	CoroutineMgr:Instance():Start()

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

end

function OnserverHandleRpcRsp(net_id, head, body)
	CoroutineMgr:Instance():Resume(head.rpc_id, CORORESULT.SUCCESS, body)
end
