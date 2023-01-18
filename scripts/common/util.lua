-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

function Serialize(obj)
	local str = ""
	local t = type(obj)
	if t == "number" then
		str = str .. obj
	elseif t == "boolean" then
		str = str .. tostring(obj)
	elseif t == "string" then
		str = str .. string.format("%q", obj)
	elseif t == "table" then
		str = str .. "{"
		for k, v in pairs(obj) do
			str = str .. "[" .. Serialize(k) .. "]=" .. Serialize(v) .. ","
		end
		local metatable = getmetatable(obj)
		if metatable ~= nil and type(metatable.__index) == "table" then
			for k, v in pairs(metatable.__index) do
				str = str .. "[" .. Serialize(k) .. "]=" .. Serialize(v) .. ","
			end
		end
		str = str .. "}"
	elseif t == "nil" then
		return nil
	else
		return nil
	end
	return str
end

function Unserialize(str)
	local t = type(str)
	if t == "nil" or str == "" then
		return nil
	elseif t == "number" or t == "string" or t == "boolean" then
		str = tostring(str)
	else
		return nil
	end
	str = "return " .. str
	local func = load(str)
	if func == nil then
		return nil
	end
	return func()
end
