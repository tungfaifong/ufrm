-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

function dump(data, showMetatable, lastCount)
	local dump_str = ""
    if type(data) ~= "table" then
        --Value
        if type(data) == "string" then
            dump_str = dump_str .. "\"" .. data .. "\""
        else
            dump_str = dump_str .. tostring(data)
        end
    else
        --Format
        local count = lastCount or 0
        count = count + 1
        dump_str = dump_str .. "{\n"
        --Metatable
        if showMetatable then
            for i = 1,count do dump_str = dump_str .. "\t" end
            local mt = getmetatable(data)
            dump_str = dump_str .. "\"__metatable\" = "
            dump_str = dump_str .. dump(mt, showMetatable, count)    -- 如果不想看到元表的元表，可将showMetatable处填nil
            dump_str = dump_str .. ",\n"     --如果不想在元表后加逗号，可以删除这里的逗号
        end
        --Key
        for key,value in pairs(data) do
            for i = 1,count do dump_str = dump_str .. "\t" end
            if type(key) == "string" then
                dump_str = dump_str .. "\"" .. key .. "\" = "
            elseif type(key) == "number" then
                dump_str = dump_str .. "[" .. key .. "] = "
            else
                dump_str = dump_str .. tostring(key)
            end
            dump_str = dump_str .. dump(value, showMetatable, count) -- 如果不想看到子table的元表，可将showMetatable处填nil
            dump_str = dump_str .. ",\n"     --如果不想在table的每一个item后加逗号，可以删除这里的逗号
        end
        --Format
        for i = 1,lastCount or 0 do dump_str = dump_str .. "\t" end
        dump_str = dump_str .. "}"
    end
    --Format
    if not lastCount then
        dump_str = dump_str .. "\n"
    end
	return dump_str
end