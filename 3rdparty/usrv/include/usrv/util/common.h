// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_COMMON_H
#define USRV_COMMON_H

#include <stdint.h>
#include <string>
#include <unistd.h>

#ifndef NAMESPACE_NAME
#define NAMESPACE_NAME usrv
#endif

#define NAMESPACE_OPEN namespace \
NAMESPACE_NAME \
{
#define NAMESPACE_CLOSE \
}

NAMESPACE_OPEN

#define Ki * 1024
#define Mi Ki Ki

using intvl_t = uint64_t;

using NETID = size_t;
using IP = std::string;
using PORT = uint16_t;

using TIMERID = size_t;

static constexpr NETID INVALID_NET_ID = -1;
static constexpr TIMERID INVALID_TIMER_ID = -1;

static constexpr uint16_t MESSAGE_HEAD_SIZE = sizeof(uint16_t);
static constexpr uint16_t MESSAGE_BODY_SIZE = UINT16_MAX;

static std::string PATH_ROOT = getcwd(NULL, 1024);

NAMESPACE_CLOSE

#endif // USRV_COMMON_H
