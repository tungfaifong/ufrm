include ../mk_base.mk

PROTO_FILE = $(wildcard ./protocol/*.proto)
$(shell $(BIN_PROTOBUF)/protoc --proto_path=./protocol --cpp_out=./protocol $(PROTO_FILE))

TARGET := libcommon.a
SUB_DIR = corotine lb_client protocol

include ../mk_lib.mk

