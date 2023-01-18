-- Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

function SendToIOClient(net_id, id, proto, body)
	local pkg = {}
	pkg.head = {}
	pkg.head.id = id
	pkg.data = pblua.Encode(proto, body)
	Send(net_id, "IOPkg", pkg)
end