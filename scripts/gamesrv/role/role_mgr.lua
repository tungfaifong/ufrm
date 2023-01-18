-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

require "role.role"

RoleMgr = RoleMgr or BaseClass()

function RoleMgr:__init()
	if self._instance then
		return
	end

	self._roles = {}
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

function RoleMgr:CreateRole(role_id)
	self._roles[role_id] = Role.New(role_id)
	self._roles[role_id]:Init()
	return self._roles[role_id]
end

