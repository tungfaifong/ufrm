// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COMMON_H
#define UFRM_COMMON_H

#include <variant>

#include "mysql++/mysql++.h"

#include "usrv/util/common.h"

#include "protocol/common.pb.h"

using namespace usrv;

using uint128_t = __uint128_t;
using variant_t = std::variant<bool, uint32_t, int32_t, uint64_t, int64_t, float, double, std::string>;
enum class VARIANTIDX
{
    BOOL = 0,
    UINT32,
    INT32,
    UINT64,
    INT64,
    FLOAT,
    DOUBLE,
    STRING
};

using NODEID = uint32_t;
using USERID = uint32_t;
using ROLEID = uint32_t;

static constexpr NODEID INVALID_NODE_ID = 0;

void ConvertVariant2PBVariant(const variant_t & v, Variant & pb_v);
void ConvertPBVariant2Variant(const Variant & pb_v, variant_t & v);
void ConvertMysqlpp2PBVariant(const mysqlpp::String & mysql_v, Variant & pb_v);
void TraceMsg(const std::string & prefix, const google::protobuf::Message * pkg);

void SignalHandler(int signo);
void SignalInit();

#endif // UFRM_COMMON_H
