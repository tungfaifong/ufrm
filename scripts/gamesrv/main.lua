-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

package.path = package.path .. ';./scripts/?.lua;./scripts/gamesrv/?.lua'

require "common.util"
require "common.net"

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
	return true
end

function Update(interval)
	return false
end

function Stop()
end

function OnConn(net_id, ip, port)
end

function OnRecv(net_id, data)
end

function OnRecvPkg(net_id, pkg)
	logger.debug("Serialize:" .. Serialize(pkg))
	local head = pkg["head"]
	local body = pkg["body"]
end

function OnDisc(net_id)
end

function OnServerHandleNormal(net_id, head, body)

end

function OnServerHandleRpcReq(net_id, head, body)

end

function OnserverHandleRpcRsp(net_id, head, body)
end
