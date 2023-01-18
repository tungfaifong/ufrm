-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

package.path = package.path .. ';./scripts/?.lua;./scripts/gamesrv/?.lua'

require "common.base_class"
require "common.define"
require "common.util"
require "common.coroutine_mgr"
require "common.db_client"
require "common.net"
require "role.role_mgr"

function Start()
	pblua.parse("common.proto")
	pblua.parse("cs.proto")
	pblua.parse("csid.proto")
	pblua.parse("io.proto")
	pblua.parse("ioid.proto")
	pblua.parse("ss.proto")
	pblua.parse("ssdcds.proto")
	pblua.parse("ssgwgs.proto")
	pblua.parse("ssid.proto")
	pblua.parse("sslcls.proto")
	pblua.parse("sspcpx.proto")

	CoroutineMgr:Instance():Start()

	return true
end

function Update(interval)
	return false
end

function Stop()
	CoroutineMgr:Instance():Stop()
end

function OnConn(net_id, ip, port)
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
	if head.id == SSID.SSID_GW_GS_FORWAR_CS_PKG then
		local pkg = body.gwgs_body.forward_cs_pkg
		local role = RoleMgr:Instance():GetRole(pkg.role_id)
		if not role then
			role = RoleMgr:Instance():CreateRole(pkg.role_id)
		end
		role:OnRecv(pkg.cs_pkg)
	end
end

function OnServerHandleRpcReq(net_id, head, body)

end

function OnserverHandleRpcRsp(net_id, head, body)
	CoroutineMgr:Instance():Resume(head.rpc_id, CORORESULT.SUCCESS, body)
end
