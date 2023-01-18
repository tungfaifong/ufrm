// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COMMON_H
#define UFRM_COMMON_H

#include "usrv/util/common.h"

using namespace usrv;

using PROCID = uint32_t;

static const IP DEFAULT_IP {"127.0.0.1"};
static constexpr PORT DEFAULT_PORT = 6666;

static constexpr PROCID INVALID_PROC_ID = 0;

#endif // UFRM_COMMON_H
