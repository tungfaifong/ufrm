-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

require("role_logic.role_logic")

LoginLogic = LoginLogic or BaseClass(RoleLogic)

function LoginLogic:__init()
	self._recv_handler = {
		[CSID.CSID_LOGIN_REQ] = function(pkg) self:_OnLoginReq(pkg) end,
		[CSID.CSID_HEART_BEAT_REQ] = function(pkg) self:_OnHeartBeatReq(pkg) end,
	}
end

function LoginLogic:_OnLoginReq(pkg)
	local login_rsp = {}
	login_rsp.result = SCLoginRsp.RESULT.SUCCESS
	SendToClient(self._role:ID(), CSID.SCID_LOGIN_RSP, {["login_rsp"] = login_rsp})
end

function LoginLogic:_OnHeartBeatReq(pkg)
	local heart_beat_rsp = {}
	SendToClient(self._role:ID(), CSID.SCID_HEART_BEAT_RSP, {["heart_beat_rsp"] = heart_beat_rsp})
end