-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

function OnAuth(data)
	local auth_rsp = {}
	auth_rsp.result = SSIOGWAuthRsp.RESULT.SUCCESS
	return SSID.SSID_IO_GW_AUTH_RSP, SSPkgHead.LOGICTYPE.CPP, "SSIOGWAuthRsp", auth_rsp
end
