include ../mk_base.mk

PROTO_FILE = $(wildcard ./protocol/*.proto)
$(shell $(BIN_PROTOBUF)/protoc --proto_path=./protocol --cpp_out=./protocol $(PROTO_FILE))

TARGET := libcommon.a
SUB_DIR = consistent_hash coroutine lb_client protocol px_client

include ../mk_lib.mk

