// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_PBLUA_H
#define UFRM_PBLUA_H

#include "google/protobuf/compiler/importer.h"
#include "google/protobuf/dynamic_message.h"
#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

namespace pblua
{
	bool EncodeSingle(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index);
	bool EncodeMultiple(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index);
	bool EncodeTable(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index);
	bool EncodeRequired(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index);
	bool EncodeOptional(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index);
	bool EncodeRepeated(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index);
	bool EncodeField(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index);
	bool EncodeMessage(google::protobuf::Message * message, const google::protobuf::Descriptor * descriptor, lua_State * L, int index);

	bool DecodeSingle(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L);
	bool DecodeMultiple(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index);
	bool DecodeTable(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L);
	bool DecodeRequired(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L);
	bool DecodeOptional(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L);
	bool DecodeRepeated(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L);
	bool DecodeField(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L);
	bool DecodeMessage(const google::protobuf::Message & message, const google::protobuf::Descriptor * descriptor, lua_State * L);

	// ret = pblua.Parse("obj.proto")
	int Parse(lua_State * L);
	// data = pblua.Encode("Obj", obj)
	int Encode(lua_State * L);
	// obj = pblua.Decode("Obj", data)
	int Decode(lua_State * L);
	
	bool LuaRef2PB(google::protobuf::Message * message, luabridge::LuaRef & lua_ref);
	bool LuaRef2PB(const std::string & proto, luabridge::LuaRef & lua_ref);
	bool LuaRef2PB(std::shared_ptr<google::protobuf::Message> & message, const std::string & proto, luabridge::LuaRef & lua_ref);
	luabridge::LuaRef PB2LuaRef(const google::protobuf::Message & message, lua_State * L);

	void Init();
}

#endif // UFRM_PBLUA_H
