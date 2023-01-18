-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

function Start()
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
end

function OnDisc(role_id)
end
