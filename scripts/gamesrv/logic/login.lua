-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

require("logic.logic")

LoginLogic = LoginLogic or BaseClass(Logic)

function LoginLogic:__init()
	self._recv_handler = {
		[CSID.CSID_GET_ROLES_REQ] = function(user_id, data) self:_OnGetRolesReq(user_id, data) end,
		[CSID.CSID_CREATE_ROLE_REQ] = function(user_id, data) self:_OnGetCreateRoleReq(user_id, data) end,
		[CSID.CSID_LOGIN_REQ] = function(user_id, data) self:_OnLoginReq(user_id, data) end,
		[CSID.CSID_LOGOUT_REQ] = function(user_id, data) self:_OnLogoutReq(user_id, data) end,
		[CSID.CSID_HEART_BEAT_REQ] = function(user_id, data) self:_OnHeartBeatReq(user_id, data) end,
	}
end

function LoginLogic:_OnGetRolesReq(user_id, data)
	CO_SPAWN(function(coro_id)
		local rows = db_client.Select(coro_id, 1, "user_role", {"user_id", "role_id"}, {["user_id"] = user_id})
		if not rows or #rows == 0 then
			logger.error("GetRolesReq Select failed user_id:{}", user_id)
			return 
		end
		local get_roles_rsp = {}
		get_roles_rsp.role_ids = {}
		for k, v in ipairs(rows) do
			table.insert(get_roles_rsp.role_ids, v.role_id)
		end
		SendToClient(user_id, CSID.SCID_GET_ROLES_RSP, "SCGetRolesRsp", get_roles_rsp)
	end)
end

function LoginLogic:_OnGetCreateRoleReq(user_id, data)
	local req = pblua.Decode("CSCreateRoleReq", data)
	CO_SPAWN(function(coro_id)
		local rows, role_id = db_client.Insert(coro_id, 1, "role", {"name"}, {req.name})

		if not rows or not role_id or rows == 0 or role_id == 0 then
			self:SendCreateRoleRsp(user_id, SCCreateRoleRsp.RESULT.FAILED)
			return
		end

		local rows, _ = db_client.Insert(coro_id, 1, "user_role", {"user_id", "role_id"}, {user_id, role_id})
		if not rows or rows == 0 then
			logger.warn("CreateRole Insert user_role failed user_id:{} role_id:{}", user_id, role_id)
		end

		self:SendCreateRoleRsp(user_id, SCCreateRoleRsp.RESULT.SUCCESS, role_id)
	end)
end

function LoginLogic:_OnLoginReq(user_id, data)
	local req = pblua.Decode("CSLoginReq", data)
	CO_SPAWN(function(coro_id)
		local rows = db_client.Select(coro_id, 1, "user_role", {"user_id", "role_id"}, {["user_id"] = user_id, ["role_id"] = req.role_id})
		if not rows or #rows == 0 then
			logger.error("LoginReq failed user_id role_id not match user_id:{} role_id:{}", user_id, req.role_id)
			self:SendLoginRsp(user_id, SCLoginRsp.RESULT.FAILED)
			return
		end

		rows = db_client.Select(coro_id, 1, "role", {}, {["role_id"] = req.role_id})
		if not rows or #rows == 0 then
			logger.error("LoginReq failed role_id not found user_id:{} role_id:{}", user_id, req.role_id)
			self:SendLoginRsp(user_id, SCLoginRsp.RESULT.FAILED)
			return
		end

		local role = RoleMgr:Instance():GetRoleByUserID(user_id)
		if not role then
			role = RoleMgr:Instance():CreateRole(user_id)
		end
		role:Unserialize(rows[1])

		self:SendLoginRsp(user_id, SCLoginRsp.RESULT.SUCCESS)
	end)
	
end

function LoginLogic:_OnLogoutReq(user_id, data)
end

function LoginLogic:_OnHeartBeatReq(user_id, data)
	local heart_beat_rsp = {}
	SendToClient(user_id, CSID.SCID_HEART_BEAT_RSP, "SCHeartBeatRsp", heart_beat_rsp)
end

function LoginLogic:SendCreateRoleRsp(user_id, result, role_id)
	local create_role_rsp = {}
	create_role_rsp.result = result
	create_role_rsp.role_id = role_id or 0
	SendToClient(user_id, CSID.SCID_CREATE_ROLE_RSP, "SCCreateRoleRsp", create_role_rsp)
end

function LoginLogic:SendLoginRsp(user_id, result)
	local login_rsp = {}
	login_rsp.result = result
	SendToClient(user_id, CSID.SCID_LOGIN_RSP, "SCLoginRsp", login_rsp)
end
