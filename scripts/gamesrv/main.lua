-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

package.path = package.path .. ';./scripts/?.lua;./scripts/gamesrv/?.lua'

require "common.util"

function Start()
	pblua.parse("common.proto")
	pblua.parse("cs.proto")
	pblua.parse("cdid.proto")
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
	local pkg = pblua.decode("SSPkg", data)
	logger.debug("Serialize:" .. Serialize(pkg))
end

function OnDisc(net_id)
end
