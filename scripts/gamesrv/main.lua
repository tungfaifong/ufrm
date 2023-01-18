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
	PBParse()

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
	if head.msg_type == SSPkgHead.MSGTYPE.NORMAL then
		OnServerHandleNormal(net_id, head, pkg.data)
	elseif head.msg_type == SSPkgHead.MSGTYPE.RPCREQ then
		OnServerHandleRpcReq(net_id, head, pkg.data)
	elseif head.msg_type == SSPkgHead.MSGTYPE.RPCRSP then
		OnserverHandleRpcRsp(net_id, head, pkg.data)
	end
end

function OnDisc(net_id)
end

function OnServerHandleNormal(net_id, head, data)
	if head.id == SSID.SSID_GW_GS_FORWAR_CS_PKG then
		local pkg = pblua.Decode("SSGWGSForwardCSPkg", data)
		local role = RoleMgr:Instance():GetRoleByUserID(pkg.user_id)
		if not role then
			role = RoleMgr:Instance():CreateRole(pkg.user_id)
		end
		role:OnRecv(pkg.cs_pkg)
	end
end

function OnServerHandleRpcReq(net_id, head, data)

end

function OnserverHandleRpcRsp(net_id, head, data)
	CoroutineMgr:Instance():Resume(head.rpc_id, CORORESULT.SUCCESS, data)
end
