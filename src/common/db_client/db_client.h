// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_DB_CLIENT_H
#define UFRM_DB_CLIENT_H

#include "px_client/px_client.h"

class DBClient
{
public:
	DBClient(PXClient & px_client);
	~DBClient() = default;

public:
	future<std::vector<std::unordered_map<std::string, variant_t>>> Select(NODEID node_id, std::string tb_name, std::vector<std::string> column, std::unordered_map<std::string, variant_t> where);
	future<bool> Insert(NODEID node_id, std::string tb_name, std::vector<std::string> column, std::vector<variant_t> value);
	future<bool> Update(NODEID node_id, std::string tb_name, std::unordered_map<std::string, variant_t> value, std::unordered_map<std::string, variant_t> where);
	future<bool> Delete(NODEID node_id, std::string tb_name, std::unordered_map<std::string, variant_t> where);

private:
	PXClient & _px_client;
};

#endif // UFRM_DB_CLIENT_H