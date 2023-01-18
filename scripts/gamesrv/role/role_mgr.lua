-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

require "role.role"

RoleMgr = RoleMgr or BaseClass()

function RoleMgr:__init()
	if self._instance then
		return
	end

	self._roles = {}
	self._user_id_2_roles = {}
end

function RoleMgr:Instance()
	if self._instance == nil then
		self._instance = self:New()
	end
	return self._instance
end

function RoleMgr:GetRole(role_id)
	return self._roles[role_id]
end

function RoleMgr:GetRoleByUserID(user_id)
	return self._user_id_2_roles[user_id]
end

function RoleMgr:CreateRole(user_id)
	self._user_id_2_roles[user_id] = Role.New()
	self._user_id_2_roles[user_id]:SetUserID(user_id)
	return self._user_id_2_roles[user_id]
end

