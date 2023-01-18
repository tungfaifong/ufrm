-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

CoroutineMgr = CoroutineMgr or BaseClass()

CORORESULT = {
	SUCCESS = 0,
	TIMEOUT = 1,
}

function CoroutineMgr:__init()
	if self._instance then
		return
	end

	self.TIMEOUT = 5000

	self._IDX = 0
	self._coros = {}
	self._timer_handler = INVALID_TIMER_ID
end

function CoroutineMgr:Start()
	self:_CheckTimeout()
end

function CoroutineMgr:Stop()
	timer.RemoveTimer(self._timer_handler)
	self._timer_handler = INVALID_TIMER_ID
	self._coros = nil
end

function CoroutineMgr:Instance()
	if self._instance == nil then
		self._instance = self:New()
	end
	return self._instance
end

function CoroutineMgr:Create(func, ...)
	local args = {...}
	self._IDX = self._IDX + 1
	self._coros[self._IDX] = {["timeout"] = engine.NowMs() + self.TIMEOUT, ["coro"] = coroutine.create(function()
		func(self._IDX, table.unpack(args))
	end)}
	self:Resume(self._IDX)
end

function CoroutineMgr:Resume(coro_id, result, data)
	if self._coros[coro_id] then
		local ret, msg = coroutine.resume(self._coros[coro_id].coro, result, data)
		if not ret or coroutine.status(self._coros[coro_id].coro) == "dead" then
			self._coros[coro_id] = nil
			if msg then
				logger.error(msg)
			end
		end
	end
end

function CoroutineMgr:_CheckTimeout()
	local now = engine.NowMs()
	for k, v in pairs(self._coros) do
		if now >= v.timeout then
			self:Resume(k, CORORESULT.TIMEOUT, nil)
		end
	end
	self._timer_handler = timer.CreateTimer(self.TIMEOUT, function() self:_CheckTimeout() end)
end

function CO_SPAWN(func, ...)
	CoroutineMgr:Instance():Create(func, ...)	
end