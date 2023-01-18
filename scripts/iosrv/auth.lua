-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

function OnAuth(body)
    local rsp_body = {}
    rsp_body.gwio_body = {}
    rsp_body.gwio_body.auth_rsp = {}
    rsp_body.gwio_body.auth_rsp.result = SSIOGWAuthRsp.RESULT.SUCCESS
    return SSID.SSID_IO_GW_AUTH_RSP, SSPkgHead.LOGICTYPE.CPP, rsp_body
end
