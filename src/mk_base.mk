CC = gcc
CXX = g++

ifeq ($(MAKECMDGOALS), debug)
	CXX_FLAGS = -Wall -fdiagnostics-color=always -g -std=c++20
else
	CXX_FLAGS = -Wall -fdiagnostics-color=always -g -std=c++20 -O2
endif

# path
PATH_ROOT = $(HOME)/projects/ufrm
PATH_SRC = $(PATH_ROOT)/src
PATH_3RDPARTY = $(PATH_ROOT)/3rdparty
PATH_BUILD = out

# include
INC_ASIO = -I$(PATH_3RDPARTY)/asio-1.20.0
INC_FMT = -I$(PATH_3RDPARTY)/fmt-8.1.0/include
INC_LUA = -I$(PATH_3RDPARTY)/lua-5.4.3/include
INC_LUA_BRIDGE = -I$(PATH_3RDPARTY)/LuaBridge-2.7
INC_SPDLOG = -I$(PATH_3RDPARTY)/spdlog-1.9.2
INC_USRV = -I$(PATH_3RDPARTY)/usrv/include -I$(PATH_3RDPARTY)/usrv/include/usrv
INC_ALL = -I$(PATH_ROOT)/src/common $(INC_ASIO) $(INC_FMT) $(INC_LUA) $(INC_LUA_BRIDGE) $(INC_SPDLOG) $(INC_USRV)

# lib
LIB_FMT = -L$(PATH_3RDPARTY)/fmt-8.1.0/lib -lfmt
LIB_LUA = -L$(PATH_3RDPARTY)/lua-5.4.3/lib -llua
LIB_USRV = -L$(PATH_3RDPARTY)/usrv/lib -lusrv
LIB_ALL = $(LIB_FMT) $(LIB_LUA) $(LIB_USRV)

LINK_FLAGS = $(LIB_ALL) -lpthread -ldl

# define
DEFINE_SPDLOG = -DSPDLOG_FMT_EXTERNAL
DEFINE += $(DEFINE_SPDLOG)