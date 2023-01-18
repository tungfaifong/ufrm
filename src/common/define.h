// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_DEFINE_H
#define UFRM_DEFINE_H

#define PKG_CREATE(pkg, PKG_TYPE) PKG_TYPE * pkg = new PKG_TYPE()

#define CORO_SPAWN(future) future.coro.resume()

#endif // UFRM_DEFINE_H
