cmake_minimum_required(VERSION 3.13)

project(ufrm LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_CXX_FLAGS_DEBUG "-Wall -fdiagnostics-color=always -g -std=c++20")
# set(CMAKE_CXX_FLAGS_DEBUG "-Wall -fdiagnostics-color=always -g")
# set(CMAKE_CXX_FLAGS_RELEASE "-Wall -fdiagnostics-color=always -g -O2") 

# path
set(PATH_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(PATH_SRC ${PATH_ROOT}/src)
set(PATH_3RD ${PATH_ROOT}/3rdparty)

set(PATH_BIN_PROTOBUF ${PATH_3RD}/protobuf-3.21.12/bin)

set(PATH_INC_3RD
	${PATH_3RD}/usrv/include
	${PATH_3RD}/usrv/include/usrv
	${PATH_3RD}/asio-1.20.0
	${PATH_3RD}/fmt-8.1.0/include
	${PATH_3RD}/jemalloc-5.3.0/include
	${PATH_3RD}/jwt-cpp/include
	${PATH_3RD}/lua-5.4.3/include
	${PATH_3RD}/LuaBridge-2.7
	${PATH_3RD}/spdlog-1.9.2
	${PATH_3RD}/toml++-3.0.1
	${PATH_3RD}/protobuf-3.21.12/include
	${PATH_3RD}/magic_enum-0.7.3
	${PATH_3RD}/mysql/include
	${PATH_3RD}/mysql++-3.3.0/include)

set(PATH_LIB_3RD
	${PATH_3RD}/usrv/lib
	${PATH_3RD}/fmt-8.1.0/lib
	${PATH_3RD}/jemalloc-5.3.0/lib
	${PATH_3RD}/lua-5.4.3/lib
	${PATH_3RD}/protobuf-3.21.12/lib
	${PATH_3RD}/mysql/lib
	${PATH_3RD}/mysql++-3.3.0/lib)

# lib
set(LIB_3RD usrv fmt jemalloc lua protobuf protobuf-lite mysqlclient mysqlpp pthread dl crypto)

set(CMAKE_INSTALL_RPATH ${PATH_3RD}/mysql++-3.3.0/lib)  
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# define
add_definitions(-DSPDLOG_FMT_EXTERNAL)
add_definitions("-Wall -fdiagnostics-color=always -g -O2")

list(APPEND CMAKE_MODULE_PATH "${PATH_ROOT}/cmake")

include(srv)

add_subdirectory(src)

install(TARGETS commonsrv dbsrv gamesrv gateway lbsrv proxy
	RUNTIME DESTINATION ${PATH_ROOT}/bin
)
