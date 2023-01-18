-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

local _class={}
 
function BaseClass(super)

	local class_type={}
	
	class_type.__init=false
	
	class_type.__super=super
	
	class_type.New=function(...)
		local obj={}
		do
			local create
			create = function(c,...)
				if c.__super then
					create(c.__super,...)
				end
				if c.__init then
					c.__init(obj,...)
				end
			end

			create(class_type,...)
		end
		setmetatable(obj,{ __index=_class[class_type] })
		return obj
	end

	local vtbl={}
	_class[class_type]=vtbl
 
	setmetatable(class_type,{__newindex=
		function(t,k,v)
			vtbl[k]=v
		end,
		__index = vtbl
	})
 
	if super then
		setmetatable(vtbl,{__index=
			function(t,k)
				local ret=_class[super][k]
				return ret
			end
		})
	end
 
	return class_type
end