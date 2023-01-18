-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

Role = Role or BaseClass()

function Role:__init()
	if self._instance then
		return
	end

	self.TB_ROLE_SERIALIZE = {
		["role_id"] = function() return self._id end,
		["name"] = function() return self._name end,
		["item"] = function() return self._role_logics["item"]:Serialize() end,
		["role_logics"] = function()
			local role_logics = {}
			for k, v in pairs(self._role_logics) do
				if k ~= "item" then
					role_logics[k] = self._role_logics[k]:Serialize()
				end
			end
			return Serialize(role_logics)
		end,
	}

	self.TB_ROLE_UNSERIALIZE = {
		["role_id"] = function(data) self._id = data end,
		["name"] = function(data) self._name = data end,
		["item"] = function(data)
			data = data == "" and "{}" or data
			self._role_logics["item"]:Unserialize(data)
		end,
		["role_logics"] = function(data)
			data = data == "" and "{}" or data
			local role_logics = Unserialize(data)
			for k, v in pairs(role_logics) do
				self._role_logics[k]:Unserialize(data)
			end
		end,
	}

	self._id = 0
	self._user_id = 0
	self._name = ""
	self._role_logics = {}
	self._recv_handler = {}

	require("role_logic.item.logic")
	self._role_logic_item = RoleLogicItem.New(self)
end

function Role:_RegisterRoleLogic(key, logic)
	self._role_logics[key] = logic
	self._role_logics[key]:OnRegister()
end

function Role:Init()
	self:_RegisterRoleLogic("item", self._role_logic_item)
end

function Role:Serialize()
	local data = {}
	for k, v in pairs(self.TB_ROLE_SERIALIZE) do
		data[k] = self.TB_ROLE_SERIALIZE[k]()
	end
	return data
end

function Role:Unserialize(data)
	for k, v in pairs(data) do
		self.TB_ROLE_UNSERIALIZE[k](v)
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

function Role:SetID(id)
	self._id = id
end

function Role:ID()
	return self._id
end

function Role:SetUserID(user_id)
	self._user_id = user_id
end

function Role:UserID()
	return self._user_id
end
