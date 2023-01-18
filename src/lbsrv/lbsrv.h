// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_LBSRV_H
#define UFRM_LBSRV_H

#include "toml++/toml.h"
#include "usrv/unit.h"

#include "common.h"

using namespace usrv;

struct Node
{
	NETID net_id;
	uint32_t load;
};

class LBSrv : public Unit, public std::enable_shared_from_this<LBSrv>
{
public:
	LBSrv(NODEID id, toml::table & config);
	virtual ~LBSrv() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

public:

private:
	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);

private:
	NODEID _id;
	toml::table & _config;

	std::unordered_map<NETID, NODEID> _nid2nid;
	std::unordered_map<NODEID, Node> _nodes[NODETYPE_ARRAYSIZE];
};

#endif // UFRM_LBSRV_H
