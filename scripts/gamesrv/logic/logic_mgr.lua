-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

require "logic.logic"

LogicMgr = LogicMgr or BaseClass()

function LogicMgr:__init()
	if self._instance then
		return
	end

	self._logics = {}
	self._recv_handler = {}

	require("logic.login")
	self._login_logic = LoginLogic.New()
end

function LogicMgr:_RegisterLogic(key, logic)
	self._logics[key] = logic
	self._logics[key]:OnRegister()
end

function LogicMgr:Instance()
	if self._instance == nil then
		self._instance = self:New()
	end
	return self._instance
end

function LogicMgr:Init()
	self:_RegisterLogic("login_logic", self._login_logic)
end

function LogicMgr:RegisterRecvHandler(handlers)
	for k, v in pairs(handlers) do
		if self._recv_handler[k] then
			logger.error("recv handler key duplicate:" .. k)
		end
		self._recv_handler[k] = v
	end
end

function LogicMgr:OnRecv(user_id, pkg)
	if not self._recv_handler[pkg.head.id] then
		logger.warn("invalid csid:" .. pkg.head.id)
		return
	end
	self._recv_handler[pkg.head.id](user_id, pkg.data)
end

function LogicMgr:GetRecvHandler(key)
	return self._recv_handler[key]
end
