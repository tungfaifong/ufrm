-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

Logic = Logic or BaseClass()

function Logic:__init()
	self._recv_handler = {}
end

function Logic:OnRegister()
	LogicMgr:Instance():RegisterRecvHandler(self._recv_handler)
end
