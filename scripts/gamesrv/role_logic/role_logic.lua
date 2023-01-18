-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

RoleLogic = RoleLogic or BaseClass()

function RoleLogic:__init(role)
	self._role = role
	self._data = {}
	self._recv_handler = {}
end

function RoleLogic:Serialize()
	return Serialize(self._data)
end

function RoleLogic:Unserialize(str)
	self._data = Unserialize(str)
end

function RoleLogic:OnRegister()
	self._role:RegisterRecvHandler(self._recv_handler)
end

