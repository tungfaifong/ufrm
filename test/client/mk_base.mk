CC = gcc
CXX = g++
AR = ar crv

CXX_FLAGS = -Wall -fdiagnostics-color=always -g -std=c++20

# ifeq ($(MAKECMDGOALS), debug)
# 	CXX_FLAGS = -Wall -fdiagnostics-color=always -g -std=c++20
# else
# 	CXX_FLAGS = -Wall -fdiagnostics-color=always -g -std=c++20 -O2
# endif

# path
PATH_ROOT = $(HOME)/projects/ufrm
PATH_SRC = $(PATH_ROOT)/src
PATH_3RDPARTY = $(PATH_ROOT)/3rdparty
PATH_BUILD = out
PATH_INSTALL = $(PATH_ROOT)/bin

# include
INC_COMMON = -I$(PATH_ROOT)/src/common
INC_USRV = -I$(PATH_3RDPARTY)/usrv/include -I$(PATH_3RDPARTY)/usrv/include/usrv
INC_ASIO = -I$(PATH_3RDPARTY)/asio-1.20.0
INC_FMT = -I$(PATH_3RDPARTY)/fmt-8.1.0/include
INC_JEMALLOC = -I$(PATH_3RDPARTY)/jemalloc-5.3.0/include
INC_LUA = -I$(PATH_3RDPARTY)/lua-5.4.3/include
INC_LUA_BRIDGE = -I$(PATH_3RDPARTY)/LuaBridge-2.7
INC_SPDLOG = -I$(PATH_3RDPARTY)/spdlog-1.9.2
INC_TOML++ = -I$(PATH_3RDPARTY)/toml++-3.0.1
INC_PROTOBUF = -I$(PATH_3RDPARTY)/protobuf-3.19.4/include
INC_MAGIC_ENUM = -I$(PATH_3RDPARTY)/magic_enum-0.7.3
INC_MYSQL = -I$(PATH_3RDPARTY)/mysql/include
INC_MYSQL++ = -I$(PATH_3RDPARTY)/mysql++-3.3.0/include
INC_ALL = $(INC_COMMON) $(INC_USRV) $(INC_ASIO) $(INC_FMT) $(INC_JEMALLOC) $(INC_LUA) $(INC_LUA_BRIDGE) $(INC_SPDLOG) $(INC_TOML++) $(INC_PROTOBUF) $(INC_MAGIC_ENUM) $(INC_MYSQL) $(INC_MYSQL++)

# lib
LIB_COMMON = -L$(PATH_ROOT)/build/src/common -lcommon
LIB_USRV = -L$(PATH_3RDPARTY)/usrv/lib -lusrv
LIB_FMT = -L$(PATH_3RDPARTY)/fmt-8.1.0/lib -lfmt
LIB_JEMALLOC = -L$(PATH_3RDPARTY)/jemalloc-5.3.0/lib -ljemalloc
LIB_LUA = -L$(PATH_3RDPARTY)/lua-5.4.3/lib -llua
LIB_PROTOBUF = -L$(PATH_3RDPARTY)/protobuf-3.19.4/lib -lprotobuf -lprotobuf-lite
LIB_MYSQL = -L$(PATH_3RDPARTY)/mysql/lib -lmysqlclient
LIB_MYSQL++ = -L$(PATH_3RDPARTY)/mysql++-3.3.0/lib -lmysqlpp -Wl,-rpath=$(PATH_3RDPARTY)/mysql++-3.3.0/lib
LIB_ALL = $(LIB_COMMON) $(LIB_USRV) $(LIB_FMT) $(LIB_JEMALLOC) $(LIB_LUA) $(LIB_PROTOBUF) $(LIB_MYSQL) $(LIB_MYSQL++)

LINK_FLAGS = $(LIB_ALL) -lpthread -ldl

# bin
BIN_PROTOBUF = $(PATH_3RDPARTY)/protobuf-3.19.4/bin

# define
DEFINE_SPDLOG = -DSPDLOG_FMT_EXTERNAL
DEFINE += $(DEFINE_SPDLOG)

# install
INSTALL = install -p -m 0777