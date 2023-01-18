-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

package.path = package.path .. ';./scripts/?.lua;./scripts/iosrv/?.lua'

require "common.util"

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

function OnDisc(net_id)
end
