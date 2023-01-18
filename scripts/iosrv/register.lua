-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

function OnRegister(net_id, data)
	local req = pblua.Decode("IORegisterReq", data)
	CO_SPAWN(function(coro_id)
		local rows, user_id = db_client.Insert(coro_id, 1, "user", {"username", "password"}, {req.username, req.password})

		if not rows or not user_id or rows == 0 or user_id == 0 then
			SendRegisterRsp(net_id, IORegisterRsp.RESULT.USERNAME_REPEATED)
			return
		end

		SendRegisterRsp(net_id, IORegisterRsp.RESULT.SUCCESS)
	end)
end

function SendRegisterRsp(net_id, result)
	local register_rsp = {}
	register_rsp.result = result
	SendToIOClient(net_id, IOID.IOID_REGISTER_RSP, "IORegisterRsp", register_rsp)
end