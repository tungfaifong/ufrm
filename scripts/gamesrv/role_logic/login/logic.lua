-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

require("role_logic.role_logic")

LoginLogic = LoginLogic or BaseClass(RoleLogic)

function LoginLogic:__init()
	self._recv_handler = {
		[CSID.CSID_LOGIN_REQ] = function() self:_OnLoginReq() end,
		[CSID.CSID_HEART_BEAT_REQ] = function() self:_OnHeartBeatReq() end,
	}
end

function LoginLogic:_OnLoginReq()
	local login_rsp = {}
	login_rsp.result = SCLoginRsp.RESULT.SUCCESS
	net.SendToClient(self._role:ID(), CSID.SCID_LOGIN_RSP, {["login_rsp"] = login_rsp})
end

function LoginLogic:_OnHeartBeatReq()
	local heart_beat_rsp = {}
	net.SendToClient(self._role:ID(), CSID.SCID_HEART_BEAT_RSP, {["heart_beat_rsp"] = heart_beat_rsp})
end