-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

require("role_logic.role_logic")

LoginLogic = LoginLogic or BaseClass(RoleLogic)

function LoginLogic:__init()
	self._recv_handler = {
		[CSID.CSID_LOGIN_REQ] = function(data) self:_OnLoginReq(data) end,
		[CSID.CSID_HEART_BEAT_REQ] = function(data) self:_OnHeartBeatReq(data) end,
	}
end

function LoginLogic:_OnLoginReq(data)
	local login_rsp = {}
	login_rsp.result = SCLoginRsp.RESULT.SUCCESS
	SendToClient(self._role:UserID(), CSID.SCID_LOGIN_RSP, "SCLoginRsp", login_rsp)
end

function LoginLogic:_OnHeartBeatReq(data)
	local heart_beat_rsp = {}
	SendToClient(self._role:UserID(), CSID.SCID_HEART_BEAT_RSP, "SCHeartBeatRsp", heart_beat_rsp)
end