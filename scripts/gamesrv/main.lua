-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

package.path = package.path .. ';./scripts/?.lua;./scripts/gamesrv/?.lua'

require "common.util"

function Start()
	pblua.parse("cs.proto")
	pblua.parse("ssgwgs.proto")
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
	local pkg = pblua.decode("SSGWGSForwardCSPkg", data)
	logger.debug("dump:" .. dump(pkg))
end

function OnDisc(net_id)
end
