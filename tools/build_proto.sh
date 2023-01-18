cd ..

protoc="./3rdparty/protobuf-3.19.4/bin/protoc"
proto_path="./src/common/proto"
out_path="./src/common/protocol"
proto_file=`ls ${proto_path}`
chmod +x $protoc
$protoc --proto_path=$proto_path --cpp_out=$out_path $proto_file
