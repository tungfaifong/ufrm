include ../mk_base.mk

PROTO_FILE = $(wildcard ./proto/*.proto)
$(shell $(BIN_PROTOBUF)/protoc --proto_path=./proto --cpp_out=./protocol $(PROTO_FILE))

TARGET := libcommon.a
SUB_DIR = consistent_hash coroutine db_client lb_client protocol px_client

include ../mk_lib.mk

