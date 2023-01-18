include ../mk_base.mk

$(shell $(BIN_PROTOBUF)/protoc --proto_path=./protocol --cpp_out=./protocol common.proto ssgwgm.proto ss.proto)

TARGET := libcommon.a
SUB_DIR = protocol

include ../mk_lib.mk

