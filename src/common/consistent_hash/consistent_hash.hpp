// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_CONSISTENT_HASH_H
#define UFRM_CONSISTENT_HASH_H

#include <map>

#include "common.h"
#include "murmurhash3.h"

class ConsistentHash
{
public:
	static constexpr size_t VIRTUAL_NODE_NUM = 1 Mi;

	ConsistentHash() {}
	~ConsistentHash() {}

	void AddNode(NODEID node_id)
	{
		for(size_t i = 0; i < VIRTUAL_NODE_NUM; ++i)
		{
			auto key = std::to_string(node_id) + "#" + std::to_string(i);
			uint128_t hash = 0;
			MurmurHash3_x64_128(key.c_str(), key.size(), 0x6384BA69, &hash);
			_virtual_nodes[hash] = node_id;
		}
	}

	void RemoveNode(NODEID node_id)
	{
		for(size_t i = 0; i < VIRTUAL_NODE_NUM; ++i)
		{
			auto key = std::to_string(node_id) + "#" + std::to_string(i);
			uint128_t hash = 0;
			MurmurHash3_x64_128(key.c_str(), key.size(), 0x6384BA69, &hash);
			_virtual_nodes.erase(hash);
		}
	}

	NODEID GetNode(const std::string & key)
	{
		uint128_t hash = 0;
		MurmurHash3_x64_128(key.c_str(), key.size(), 0x6384BA69, &hash);
		auto iter = _virtual_nodes.lower_bound(hash);
		if(iter == _virtual_nodes.end())
		{
			return INVALID_NODE_ID;
		}
		return iter->second;
	}

private:
	std::map<uint128_t, NODEID> _virtual_nodes;
};

#endif // UFRM_CONSISTENT_HASH_H
