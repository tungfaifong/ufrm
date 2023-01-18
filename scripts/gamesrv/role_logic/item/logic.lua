-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

require("role_logic.role_logic")

RoleLogicItem = RoleLogicItem or BaseClass(RoleLogic)

function RoleLogicItem:__init(role)
	self._recv_handler = {}
end
