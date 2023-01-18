-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

package.path = package.path .. ';./scripts/?.lua;./scripts/gamesrv/?.lua'

require "common.util"

function Start()
	pblua.parse("cs.proto")
	return true
end

function Update(interval)
	return false
end

function Stop()
end

function OnConn(role_id, ip, port)
end

function OnRecv(role_id, data)
	logger.debug("OnRecv role_id:" .. role_id .. " data:" .. data)
	local pkg = pblua.decode("CSPkg", data)
	logger.debug("pkg headid:" .. pkg.head.id .. "")
	logger.debug("dump" .. dump(pkg))
end

function OnDisc(role_id)
end
