// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "google/protobuf/compiler/importer.h"
#include "google/protobuf/dynamic_message.h"
#include "lua.hpp"

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

	bool encode_field(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index);
	bool encode_message(google::protobuf::Message * message, const google::protobuf::Descriptor * descriptor, lua_State * L, int index);
	bool decode_field(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L);
	bool decode_message(const google::protobuf::Message & message, const google::protobuf::Descriptor * descriptor, lua_State * L);

	// encode
	bool encode_single(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
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
				encode_message(submessage, field->message_type(), L, index);
			}
			break;
		default:
			return false;
		}
		return true;
	}

	bool encode_multiple(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
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
				encode_message(submessage, field->message_type(), L, index);
			}
			break;
		default:
			return false;
		}
		return true;
	}

	bool encode_table(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
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
			encode_field(submessage, key, L, lua_absindex(L, -2));
			encode_field(submessage, value, L, lua_absindex(L, -1));
			lua_pop(L, 1);
		}

		return true;
	}

	bool encode_required(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
	{
		if (lua_isnil(L, index))
		{
			return true;
		}

		return encode_single(message, field, L, index);
	}

	bool encode_optional(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
	{
		if (lua_isnil(L, index))
		{
			return true;
		}

		return encode_single(message, field, L, index);
	}

	bool encode_repeated(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
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
			encode_multiple(message, field, L, lua_absindex(L, -1));
			lua_pop(L, 1);
		}
		return true;
	}

	bool encode_field(google::protobuf::Message * message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
	{
		if (field->is_map())
			return encode_table(message, field, L, index);
		else if (field->is_required())
			return encode_required(message, field, L, index);
		else if (field->is_optional())
			return encode_optional(message, field, L, index);
		else if (field->is_repeated())
			return encode_repeated(message, field, L, index);
		else
			return false;
	}

	bool encode_message(google::protobuf::Message * message, const google::protobuf::Descriptor * descriptor, lua_State * L, int index)
	{
		if (!lua_istable(L, index))
		{
			return false;
		}

		for(auto i = 0; i < descriptor->field_count(); i++)
		{
			auto field = descriptor->field(i);
			lua_getfield(L, index, field->name().c_str());
			encode_field(message, field, L, lua_absindex(L, -1));
			lua_pop(L, 1);
		}

		return true;
	}

	// decode
	bool decode_single(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L)
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
				decode_message(submessage, field->message_type(), L);
			}
			break;
		default:
			return false;
		}
		return true;
	}

	bool decode_multiple(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L, int index)
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
				decode_message(submessage, field->message_type(), L);
			}
			break;
		default:
			return false;
		}
		return true;
	}

	bool decode_table(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L)
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
			decode_field(submessage, key, L);
			decode_field(submessage, value, L);
			lua_settable(L, -3);
		}
		return true;
	}

	bool decode_required(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L)
	{
		auto reflection = message.GetReflection();
		if (!reflection->HasField(message, field))
		{
			return true;
		}

		return decode_single(message, field, L);
	}

	bool decode_optional(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L)
	{
		auto reflection = message.GetReflection();
		if (field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE && !reflection->HasField(message, field))
		{
			lua_pushnil(L);
			return true;
		}

		return decode_single(message, field, L);
	}

	bool decode_repeated(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L)
	{
		auto reflection = message.GetReflection();
		auto field_size = reflection->FieldSize(message, field);
		lua_createtable(L, field_size, 0);
		for (auto index = 0; index < field_size; index++)
		{
			decode_multiple(message, field, L, index);
			lua_seti(L, -2, index + 1);
		}
		return true;
	}

	bool decode_field(const google::protobuf::Message & message, const google::protobuf::FieldDescriptor * field, lua_State * L)
	{
		if (field->is_map())
			return decode_table(message, field, L);
		else if (field->is_required())
			return decode_required(message, field, L);
		else if (field->is_optional())
			return decode_optional(message, field, L);
		else if (field->is_repeated())
			return decode_repeated(message, field, L);
		else
			return false;
	}

	bool decode_message(const google::protobuf::Message & message, const google::protobuf::Descriptor * descriptor, lua_State * L)
	{
		auto field_count = descriptor->field_count();
		lua_createtable(L, 0, field_count);
		for(auto i = 0; i < field_count; i++)
		{
			auto field = descriptor->field(i);
			decode_field(message, field, L);
			lua_setfield(L, -2, field->name().c_str());
		}
		return true;
	}

	// common
	bool _parse(const char* file, lua_State * L)
	{
		importer->Import(file);

		return true;
	}

	bool _encode(const char* proto, lua_State * L, int index, char* data, size_t* size)
	{
		auto descriptor = importer->pool()->FindMessageTypeByName(proto);
		auto prototype = message_factory->GetPrototype(descriptor);

		index = lua_absindex(L, index);
		auto message = std::unique_ptr<google::protobuf::Message>(prototype->New());
		encode_message(message.get(), descriptor, L, index);

		message->SerializeToArray(data, *size);
		*size = message->ByteSizeLong();

		return true;
	}

	bool _decode(const char* proto, lua_State * L, const char* data, size_t size)
	{
		auto descriptor = importer->pool()->FindMessageTypeByName(proto);
		auto prototype = message_factory->GetPrototype(descriptor);

		auto message = std::unique_ptr<google::protobuf::Message>(prototype->New());
		message->ParseFromArray(data, size);

		return decode_message(*message.get(), descriptor, L);
	}

	// ret = pblua.parse("obj.proto")
	static int parse(lua_State * L)
	{
		assert(lua_gettop(L) == 1);
		luaL_checktype(L, 1, LUA_TSTRING);
		auto file = lua_tostring(L, 1);
		if (!_parse(file, L))
		{
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, true);
		return 1;
	}

	// data = pblua.encode("Obj", obj)
	static int encode(lua_State * L)
	{
		assert(lua_gettop(L) == 2);
		luaL_checktype(L, 1, LUA_TSTRING);
		luaL_checktype(L, 2, LUA_TTABLE);
		auto proto = lua_tostring(L, 1);
		if (!_encode(proto, L, 2, 0, 0))
		{
			return 0;
		}

		return lua_gettop(L) - 2;
	}

	// obj = pblua.decode("Obj", data)
	static int decode(lua_State * L)
	{
		assert(lua_gettop(L) == 2);
		size_t size = 0;
		luaL_checktype(L, 1, LUA_TSTRING);
		auto proto = lua_tostring(L, 1);
		luaL_checktype(L, 2, LUA_TSTRING);
		auto data = lua_tolstring(L, 2, &size);
		if (!_decode(proto, L, data, size))
		{
			return 0;
		}

		return lua_gettop(L) - 2;
	}

	static void init()
	{
		source_tree = new google::protobuf::compiler::DiskSourceTree();
		source_tree->MapPath("", "./");
		error_collector = new ErrorCollector();
		importer = new google::protobuf::compiler::Importer(source_tree, error_collector);
		message_factory = new google::protobuf::DynamicMessageFactory();
	}
}
