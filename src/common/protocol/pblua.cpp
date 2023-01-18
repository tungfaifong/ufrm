// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "pblua.h"

#include "usrv/util/common.h"

namespace pblua
{
	class ErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector
	{
	public:
		void AddError(const std::string & filename, int line, int column, const std::string & message) {}
	};

	static google::protobuf::compiler::DiskSourceTree * source_tree = nullptr;
	static ErrorCollector * error_collector = nullptr;
	static google::protobuf::compiler::Importer * importer = nullptr;
	static google::protobuf::DynamicMessageFactory * message_factory = nullptr;

	// encode
	bool EncodeSingle(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
	{
		auto reflection = message->GetReflection();
		switch (field->cpp_type())
		{
		case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
			reflection->SetDouble(message, field, (double)lua_tonumber(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
			reflection->SetFloat(message, field, (float)lua_tonumber(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
			reflection->SetInt32(message, field, (int32_t)lua_tointeger(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
			reflection->SetUInt32(message, field, (uint32_t)lua_tointeger(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
			reflection->SetInt64(message, field, (int64_t)lua_tointeger(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
			reflection->SetUInt64(message, field, (uint64_t)lua_tointeger(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
			reflection->SetEnumValue(message, field, (int)lua_tointeger(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
			reflection->SetBool(message, field, lua_toboolean(L, index) != 0);
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
			{
				size_t length = 0;
				auto bytes = lua_tolstring(L, index, &length);
				reflection->SetString(message, field, std::string(bytes, length));
			}
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
			{
				auto submessage = reflection->MutableMessage(message, field);
				EncodeMessage(submessage, field->message_type(), L, index);
			}
			break;
		default:
			return false;
		}
		return true;
	}

	bool EncodeMultiple(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
	{
		auto reflection = message->GetReflection();
		switch (field->cpp_type())
		{
		case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
			reflection->AddDouble(message, field, (double)lua_tonumber(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
			reflection->AddFloat(message, field, (float)lua_tonumber(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
			reflection->AddInt32(message, field, (int32_t)lua_tointeger(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
			reflection->AddUInt32(message, field, (uint32_t)lua_tointeger(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
			reflection->AddInt64(message, field, (int64_t)lua_tointeger(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
			reflection->AddUInt64(message, field, (uint64_t)lua_tointeger(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
			reflection->AddEnumValue(message, field, (int)lua_tointeger(L, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
			reflection->AddBool(message, field, lua_toboolean(L, index) != 0);
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
			{
				size_t length = 0;
				auto bytes = lua_tolstring(L, index, &length);
				reflection->AddString(message, field, std::string(bytes, length));
			}
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
			{
				auto submessage = reflection->AddMessage(message, field);
				EncodeMessage(submessage, field->message_type(), L, index);
			}
			break;
		default:
			return false;
		}
		return true;
	}

	bool EncodeTable(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
	{
		if (lua_isnil(L, index))
		{
			return true;
		}

		if (!lua_istable(L, index))
		{
			return false;
		}

		auto reflection = message->GetReflection();
		auto descriptor = field->message_type();
		auto key = descriptor->field(0);
		auto value = descriptor->field(1);

		lua_pushnil(L);
		while (lua_next(L, index))
		{
			auto submessage = reflection->AddMessage(message, field);
			EncodeField(submessage, key, L, lua_absindex(L, -2));
			EncodeField(submessage, value, L, lua_absindex(L, -1));
			lua_pop(L, 1);
		}

		return true;
	}

	bool EncodeRequired(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
	{
		if (lua_isnil(L, index))
		{
			return true;
		}

		return EncodeSingle(message, field, L, index);
	}

	bool EncodeOptional(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
	{
		if (lua_isnil(L, index))
		{
			return true;
		}

		return EncodeSingle(message, field, L, index);
	}

	bool EncodeRepeated(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
	{
		if (lua_isnil(L, index))
		{
			return true;
		}

		if (!lua_istable(L, index))
		{
			return false;
		}

		auto count = (int)luaL_len(L, index);
		for(auto i = 0; i < count; i++)
		{
			lua_geti(L, index, i + 1);
			EncodeMultiple(message, field, L, lua_absindex(L, -1));
			lua_pop(L, 1);
		}
		return true;
	}

	bool EncodeField(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
	{
		if (field->is_map())
			return EncodeTable(message, field, L, index);
		else if (field->is_required())
			return EncodeRequired(message, field, L, index);
		else if (field->is_optional())
			return EncodeOptional(message, field, L, index);
		else if (field->is_repeated())
			return EncodeRepeated(message, field, L, index);
		else
			return false;
	}

	bool EncodeMessage(google::protobuf::Message * message, const google::protobuf::Descriptor * descriptor, lua_State * L, int index)
	{
		if (!lua_istable(L, index))
		{
			return false;
		}

		for(auto i = 0; i < descriptor->field_count(); i++)
		{
			auto field = descriptor->field(i);
			lua_getfield(L, index, field->name().c_str());
			EncodeField(message, field, L, lua_absindex(L, -1));
			lua_pop(L, 1);
		}

		return true;
	}

	// decode
	bool DecodeSingle(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L)
	{
		auto reflection = message.GetReflection();
		switch (field->cpp_type())
		{
		case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
			lua_pushnumber(L, reflection->GetDouble(message, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
			lua_pushnumber(L, reflection->GetFloat(message, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
			lua_pushinteger(L, reflection->GetInt32(message, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
			lua_pushinteger(L, reflection->GetUInt32(message, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
			lua_pushinteger(L, reflection->GetInt64(message, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
			lua_pushinteger(L, reflection->GetUInt64(message, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
			lua_pushinteger(L, reflection->GetEnumValue(message, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
			lua_pushboolean(L, reflection->GetBool(message, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
			{
				auto value = reflection->GetString(message, field);
				lua_pushlstring(L, value.c_str(), value.size());
			}
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
			{
				const auto & submessage = reflection->GetMessage(message, field);
				DecodeMessage(submessage, field->message_type(), L);
			}
			break;
		default:
			return false;
		}
		return true;
	}

	bool DecodeMultiple(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
	{
		auto reflection = message.GetReflection();
		switch (field->cpp_type())
		{
		case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
			lua_pushnumber(L, reflection->GetRepeatedDouble(message, field, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
			lua_pushnumber(L, reflection->GetRepeatedFloat(message, field, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
			lua_pushinteger(L, reflection->GetRepeatedInt32(message, field, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
			lua_pushinteger(L, reflection->GetRepeatedUInt32(message, field, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
			lua_pushinteger(L, reflection->GetRepeatedInt64(message, field, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
			lua_pushinteger(L, reflection->GetRepeatedUInt64(message, field, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
			lua_pushinteger(L, reflection->GetRepeatedEnumValue(message, field, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
			lua_pushboolean(L, reflection->GetRepeatedBool(message, field, index));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
			{
				auto value = reflection->GetRepeatedString(message, field, index);
				lua_pushlstring(L, value.c_str(), value.size());
			}
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
			{
				const auto & submessage = reflection->GetRepeatedMessage(message, field, index);
				DecodeMessage(submessage, field->message_type(), L);
			}
			break;
		default:
			return false;
		}
		return true;
	}

	bool DecodeTable(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L)
	{
		auto reflection = message.GetReflection();
		auto field_size = reflection->FieldSize(message, field);
		auto descriptor = field->message_type();
		auto key = descriptor->field(0);
		auto value = descriptor->field(1);

		lua_createtable(L, 0, field_size);
		for(auto index = 0; index < field_size; index++)
		{
			const auto & submessage = reflection->GetRepeatedMessage(message, field, index);
			DecodeField(submessage, key, L);
			DecodeField(submessage, value, L);
			lua_settable(L, -3);
		}
		return true;
	}

	bool DecodeRequired(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L)
	{
		auto reflection = message.GetReflection();
		if (!reflection->HasField(message, field))
		{
			return true;
		}

		return DecodeSingle(message, field, L);
	}

	bool DecodeOptional(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L)
	{
		auto reflection = message.GetReflection();
		if (!reflection->HasField(message, field))
		{
			lua_pushnil(L);
			return true;
		}

		return DecodeSingle(message, field, L);
	}

	bool DecodeRepeated(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L)
	{
		auto reflection = message.GetReflection();
		auto field_size = reflection->FieldSize(message, field);
		lua_createtable(L, field_size, 0);
		for (auto index = 0; index < field_size; index++)
		{
			DecodeMultiple(message, field, L, index);
			lua_seti(L, -2, index + 1);
		}
		return true;
	}

	bool DecodeField(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L)
	{
		if (field->is_map())
			return DecodeTable(message, field, L);
		else if (field->is_required())
			return DecodeRequired(message, field, L);
		else if (field->is_optional())
			return DecodeOptional(message, field, L);
		else if (field->is_repeated())
			return DecodeRepeated(message, field, L);
		else
			return false;
	}

	bool DecodeMessage(const google::protobuf::Message & message, const google::protobuf::Descriptor * descriptor, lua_State * L)
	{
		auto field_count = descriptor->field_count();
		lua_createtable(L, 0, field_count);
		for(auto i = 0; i < field_count; i++)
		{
			auto field = descriptor->field(i);
			DecodeField(message, field, L);
			lua_setfield(L, -2, field->name().c_str());
		}
		return true;
	}

	// common
	bool _ParseEnum(const google::protobuf::EnumDescriptor * enum_desc, lua_State * L)
	{
		auto enum_count = enum_desc->value_count();
		lua_createtable(L, 0, enum_count);
		for (auto i = 0; i < enum_count; i++)
		{
			auto value_desc = enum_desc->value(i);
			lua_pushinteger(L, value_desc->number());
			lua_setfield(L, -2, value_desc->name().c_str());
		}

		return true;
	}

	bool _ParseMessage(const google::protobuf::Descriptor* message_desc, lua_State* L)
	{
		lua_newtable(L);

		for (auto i = 0; i < message_desc->enum_type_count(); i++)
		{
			auto enum_desc = message_desc->enum_type(i);
			_ParseEnum(enum_desc, L);
			lua_setfield(L, -2, enum_desc->name().c_str());
		}

		for (auto i = 0; i < message_desc->nested_type_count(); i++)
		{
			auto n_message_desc = message_desc->nested_type(i);
			_ParseMessage(n_message_desc, L);
			lua_setfield(L, -2, n_message_desc->name().c_str());
		}

		return true;
	}

	bool _Parse(const char* file, lua_State * L)
	{
		auto file_desc = importer->Import(file);

		for (auto i = 0; i < file_desc->enum_type_count(); i++)
		{
			auto enum_desc = file_desc->enum_type(i);
			_ParseEnum(file_desc->enum_type(i), L);
			lua_setglobal(L, enum_desc->name().c_str());
		}

		for (auto i = 0; i < file_desc->message_type_count(); i++)
		{
			auto message_desc = file_desc->message_type(i);
			_ParseMessage(message_desc, L);
			lua_setglobal(L, message_desc->name().c_str());
		}

		return true;
	}

	bool _Encode(const char* proto, lua_State * L, int index, char* data, size_t* size)
	{
		auto descriptor = importer->pool()->FindMessageTypeByName(proto);
		auto prototype = message_factory->GetPrototype(descriptor);

		index = lua_absindex(L, index);
		auto message = std::unique_ptr<google::protobuf::Message>(prototype->New());
		EncodeMessage(message.get(), descriptor, L, index);

		if (data && size)
		{
			message->SerializeToArray(data, *size);
			*size = message->ByteSizeLong();
		}
		else 
		{
			std::string data;
			size_t size;
			message->SerializeToString(&data);
			size = message->ByteSizeLong();
			lua_pushlstring(L, data.c_str(), data.size());
			lua_pushinteger(L, size);
		}

		return true;
	}

	bool _Decode(const char* proto, lua_State * L, const char* data, size_t size)
	{
		auto descriptor = importer->pool()->FindMessageTypeByName(proto);
		auto prototype = message_factory->GetPrototype(descriptor);

		auto message = std::unique_ptr<google::protobuf::Message>(prototype->New());
		message->ParseFromArray(data, size);

		return DecodeMessage(*message.get(), descriptor, L);
	}

	int Parse(lua_State * L)
	{
		assert(lua_gettop(L) == 1);
		luaL_checktype(L, 1, LUA_TSTRING);
		auto file = lua_tostring(L, 1);
		if (!_Parse(file, L))
		{
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, true);
		return 1;
	}

	int Encode(lua_State * L)
	{
		assert(lua_gettop(L) == 2);
		luaL_checktype(L, 1, LUA_TSTRING);
		luaL_checktype(L, 2, LUA_TTABLE);
		auto proto = lua_tostring(L, 1);
		if (!_Encode(proto, L, 2, 0, 0))
		{
			return 0;
		}

		return lua_gettop(L) - 2;
	}

	int Decode(lua_State * L)
	{
		assert(lua_gettop(L) == 2);
		size_t size = 0;
		luaL_checktype(L, 1, LUA_TSTRING);
		auto proto = lua_tostring(L, 1);
		luaL_checktype(L, 2, LUA_TSTRING);
		auto data = lua_tolstring(L, 2, &size);
		if (!_Decode(proto, L, data, size))
		{
			return 0;
		}

		return lua_gettop(L) - 2;
	}

	bool LuaRef2PB(google::protobuf::Message * message, luabridge::LuaRef & lua_ref)
	{
		auto descriptor = message->GetDescriptor();
		lua_ref.push();
		return EncodeMessage(message, descriptor, lua_ref.state(), lua_absindex(lua_ref.state(), -1));
	}

	luabridge::LuaRef PB2LuaRef(const google::protobuf::Message & message, lua_State * L)
	{
		auto descriptor = message.GetDescriptor();

		DecodeMessage(message, descriptor, L);
		
		return luabridge::LuaRef::fromStack(L);
	}

	void Init()
	{
		source_tree = new google::protobuf::compiler::DiskSourceTree();
		source_tree->MapPath("", usrv::PATH_ROOT + "/src/common/proto/");
		error_collector = new ErrorCollector();
		importer = new google::protobuf::compiler::Importer(source_tree, error_collector);
		message_factory = new google::protobuf::DynamicMessageFactory();
	}
}
