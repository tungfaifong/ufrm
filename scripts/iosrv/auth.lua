-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

local private_key = [[-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAooiorOvRkCFhR+2iQ2CIoBFG50qCl2DxNhOcTULpK6x7O+nH
7sH/CANgFKpH22LoEKNMawiZN0z9/zGPgR8CrnWH2MDDqmQ6ribT1kv3WOq8VpgO
fTJ1ZpMLAmh4F+QJF6O2uGsiio30BJoxAFn4svTO++ocRn43MGa/L4tkCvS9s/O7
U4oUzXItJYJtis7h3EChlPB45WBapymtXhE8ubiF50AgFAx3Ac6EKEp+g8CE1oue
vrOuDBra0jpLdYb7iKZWA92zp8/MhWa+DylwrYE0UgHQc29Jepn2NJtpVp2yfJ3i
KNUSBsVyG/ZI4vqQwjNYKNdhRAj4GHDeJ18/IwIDAQABAoIBABmxcLilLqOSI0az
kgsEn5RuJr85UBuN/5OE1hiAk0uMII8ADdL4Z9BkeDPMFz/fCsBWkCX7Xf0X9q74
rq7CsDHT8n2ZO1UcMwqEqMgfiiJMOSmXL/MqntRwOjVPPmzIZ4eNIb1Vckhk2EZB
+a0Mi/v+Jxo7j+y01xSXqXFin5X7idcYdd+kEUjGErjK9zq34syf9yx79XzMWvXx
H/LjnnjmUz1ccs7neYC9Uq7Bh0EMq/STbvkZYAvANLvDIZuPaONwvHKfYSR/rned
IK3hAgbb59WJE8qsDfo4LSAFd1fl0MELHyWlx6cWfQCcIvRspZHSwxqij866yDig
U8xlLEECgYEA1BzjTAYW4q3VD+5Pu7Ijxjrgb3UPewgVi1zc8jW52aeSAQbe8Y0/
XwYBip84VT2klI/tJL/mAq/1PdfSpOnKBcdqUxhTteLxv7g7jns6N8HHYp+3OSdu
71cYVNT8oBW9R5b1XRi5bXEJZx8Kjfx/96CIn6b/YedCR38MYX3hyDUCgYEAxCmw
POQJaY2KpKgh5B177EXHsCL24sNZpFYPTb1OdSVnpvwQIJsbKydR7l1HYyGjbPu2
TvxsLuPy/okiuVutCQ6vqLGF8c/q032cc4G0tdwo8xG/+24BuEdVKvTOpCdyGBiQ
mGq5EMuM5KjgklyBd8LFSUaYra/v1XtUwrkVRPcCgYEAxuoMUhgH0C36d8UhqsBF
AjnmjT929AbBxX6hiNbPYgjeR+48lsPCIQeN3ISyHGYBeHx8hJEdT3nZf1RtKtA/
7ylyUVin8BNSaFuKJnTzRKSJqbhj58pOI09G9w8BldKtPXyYvuRXyAVQLw+SI1Me
Yod+eFHwfsyTgF7pqmeVm90CgYBG6TcPW/PXF1aWHT1bJBlnGPaNwqXy5WFjWcTg
el2BVYs5vwbpcoiIugcRHuEugbt9KyUFOpS1AqwVvWczEGPl0UxItBvShTHDUDYd
PzcfQr3+2FzJPEQc5JRyGdsVVjkVYtPqdTH/U0XUZwP/hLc8nskIqx+IMgCF8LZ8
ScHZkwKBgH1mFjlKohdLo+DwDlruy8I46r0j+3t5wSbQlRt4M5b/qsn3VTlRkgIE
6d/oIsJV8Q3m2y5r7/TZtZB8Ghaiy72ZgmJROjAhKvo7Amb6l+dFsfnMzrxc/eDL
laIcHvbgQYVf649+9uYZ9zlEyap9BdGZLpsQgZVkuPEEiS/GaYyb
-----END RSA PRIVATE KEY-----]]

local public_key = [[-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAooiorOvRkCFhR+2iQ2CI
oBFG50qCl2DxNhOcTULpK6x7O+nH7sH/CANgFKpH22LoEKNMawiZN0z9/zGPgR8C
rnWH2MDDqmQ6ribT1kv3WOq8VpgOfTJ1ZpMLAmh4F+QJF6O2uGsiio30BJoxAFn4
svTO++ocRn43MGa/L4tkCvS9s/O7U4oUzXItJYJtis7h3EChlPB45WBapymtXhE8
ubiF50AgFAx3Ac6EKEp+g8CE1ouevrOuDBra0jpLdYb7iKZWA92zp8/MhWa+Dylw
rYE0UgHQc29Jepn2NJtpVp2yfJ3iKNUSBsVyG/ZI4vqQwjNYKNdhRAj4GHDeJ18/
IwIDAQAB
-----END PUBLIC KEY-----]]

local auth_user = {}

function OnAuth(data)
	local req = pblua.Decode("SSGWIOAuthReq", data)
	local auth_rsp = {}
	if not auth_user[req.user_id] or auth_user[req.user_id] ~= req.token then
		auth_rsp.result = SSIOGWAuthRsp.RESULT.FAILED
		return SSID.SSID_IO_GW_AUTH_RSP, SSPkgHead.LOGICTYPE.CPP, "SSIOGWAuthRsp", auth_rsp
	end
	auth_user[req.user_id] = nil
	auth_rsp.result = SSIOGWAuthRsp.RESULT.SUCCESS
	return SSID.SSID_IO_GW_AUTH_RSP, SSPkgHead.LOGICTYPE.CPP, "SSIOGWAuthRsp", auth_rsp
end

function OnIOAuth(net_id, data)
	local req = pblua.Decode("IOAuthReq", data)
	CO_SPAWN(function(coro_id)
		local rows = db_client.Select(coro_id, 1, "user", {"user_id"}, {["username"] = req.username, ["password"] = req.password})
		if not rows or #rows == 0 then
			SendIOAuthRsp(net_id, IOAuthRsp.RESULT.FAILED)
			return 
		end

		logger.info(Serialize(rows))
		local user_id = rows[1].user_id
		local token = jwt.Create(private_key, user_id, 120)

		auth_user[user_id] = token

		SendIOAuthRsp(net_id, IOAuthRsp.RESULT.SUCCESS, user_id, token)
	end)
end

function SendIOAuthRsp(net_id, result, user_id, token)
	user_id = user_id or 0
	token = token or ""
	local ioauth_rsp = {}
	ioauth_rsp.result = result
	ioauth_rsp.user_id = user_id
	ioauth_rsp.token = token
	SendToIOClient(net_id, IOID.IOID_AUTH_RSP, "IOAuthRsp", ioauth_rsp)
end
