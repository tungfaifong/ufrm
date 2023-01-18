-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

Role = Role or BaseClass()

function Role:__init(role_id)
	if self._instance then
		return
	end

	self._id = role_id
	self._role_logics = {}
	self._recv_handler = {}

	require("role_logic.login.logic")
	self._login_logic = LoginLogic.New(self)
end

function Role:Init()
	self:_RegisterRoleLogic("login_logic", self._login_logic)
end

function Role:Serialize()
	local tb = {}
	tb.id = self._id
	tb.role_logics = {}
	for k, v in pairs(self._role_logics) do
		tb.role_logics[k] = v:Serialize()
	end
	return Serialize(tb)
end

function Role:Unserialize(str)
	local tb = Unserialize(str)
	self._id = tb.id
	for k, v in tb.role_logics do
		self._role_logics[k]:Unserialize(v)
	end
end

function Role:OnRecv(pkg)
	if not self._recv_handler[pkg.head.id] then
		logger.warn("invalid csid:" .. pkg.head.id)
		return
	end
	self._recv_handler[pkg.head.id](pkg.data)
end

function Role:RegisterRecvHandler(handlers)
	for k, v in pairs(handlers) do
		if self._recv_handler[k] then
			logger.error("recv handler key duplicate:" .. k)
		end
		self._recv_handler[k] = v
	end
end

function Role:ID()
	return self._id
end

function Role:_RegisterRoleLogic(key, logic)
	self._role_logics[key] = logic
	self._role_logics[key]:OnRegister()
end

