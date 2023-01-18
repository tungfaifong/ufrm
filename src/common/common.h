// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COMMON_H
#define UFRM_COMMON_H

#include "usrv/util/common.h"

using namespace usrv;

using NODEID = uint32_t;

static const IP DEFAULT_IP {""};
static constexpr PORT DEFAULT_PORT = 0;

static constexpr NODEID INVALID_NODE_ID = 0;

#endif // UFRM_COMMON_H
