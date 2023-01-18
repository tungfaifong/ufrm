// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COMMON_H
#define UFRM_COMMON_H

#include "usrv/util/common.h"

using namespace usrv;

using NODEID = uint32_t;

static const IP DEFAULT_IP {"127.0.0.1"};
static constexpr PORT DEFAULT_PORT = 6666;

static constexpr NODEID INVALID_NODE_ID = 0;

#define CREATE_PKG(pkg, PKG_TYPE) PKG_TYPE * pkg = new PKG_TYPE()

#endif // UFRM_COMMON_H
