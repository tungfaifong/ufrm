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
	coroutine.resume(self._coros[self._IDX].coro)
end

function CoroutineMgr:Resume(coro_id, result, data)
	if self._coros[coro_id] then
		if not coroutine.resume(self._coros[coro_id].coro, result, data) then
			self._coros[coro_id] = nil
		end
	end
end

function CoroutineMgr:_CheckTimeout()
	local now = engine.NowMs()
	for k, v in pairs(self._coros) do
		if now >= v.timeout then
			if not coroutine.resume(self._coros[k].coro, CORORESULT.TIMEOUT, nil) then
				self._coros[k] = nil
			end
		end
	end
	self._timer_handler = timer.CreateTimer(self.TIMEOUT, function() self:_CheckTimeout() end)
end

function CO_SPAWN(func, ...)
	CoroutineMgr:Instance():Create(func, ...)	
end