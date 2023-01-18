// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ssgwgs.proto

#include "ssgwgs.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
constexpr SSGWGSInit::SSGWGSInit(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized){}
struct SSGWGSInitDefaultTypeInternal {
  constexpr SSGWGSInitDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~SSGWGSInitDefaultTypeInternal() {}
  union {
    SSGWGSInit _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT SSGWGSInitDefaultTypeInternal _SSGWGSInit_default_instance_;
constexpr SSGWGSHertBeatReq::SSGWGSHertBeatReq(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized){}
struct SSGWGSHertBeatReqDefaultTypeInternal {
  constexpr SSGWGSHertBeatReqDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~SSGWGSHertBeatReqDefaultTypeInternal() {}
  union {
    SSGWGSHertBeatReq _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT SSGWGSHertBeatReqDefaultTypeInternal _SSGWGSHertBeatReq_default_instance_;
constexpr SSGSGWHertBeatRsp::SSGSGWHertBeatRsp(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized){}
struct SSGSGWHertBeatRspDefaultTypeInternal {
  constexpr SSGSGWHertBeatRspDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~SSGSGWHertBeatRspDefaultTypeInternal() {}
  union {
    SSGSGWHertBeatRsp _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT SSGSGWHertBeatRspDefaultTypeInternal _SSGSGWHertBeatRsp_default_instance_;
constexpr SSGWGSForwardCSPkg::SSGWGSForwardCSPkg(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : cs_pkg_(nullptr)
  , role_id_(0u)
  , game_id_(0u){}
struct SSGWGSForwardCSPkgDefaultTypeInternal {
  constexpr SSGWGSForwardCSPkgDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~SSGWGSForwardCSPkgDefaultTypeInternal() {}
  union {
    SSGWGSForwardCSPkg _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT SSGWGSForwardCSPkgDefaultTypeInternal _SSGWGSForwardCSPkg_default_instance_;
constexpr SSGWGSPkgBody::SSGWGSPkgBody(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : _oneof_case_{}{}
struct SSGWGSPkgBodyDefaultTypeInternal {
  constexpr SSGWGSPkgBodyDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~SSGWGSPkgBodyDefaultTypeInternal() {}
  union {
    SSGWGSPkgBody _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT SSGWGSPkgBodyDefaultTypeInternal _SSGWGSPkgBody_default_instance_;
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_ssgwgs_2eproto[5];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_ssgwgs_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_ssgwgs_2eproto = nullptr;

const uint32_t TableStruct_ssgwgs_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::SSGWGSInit, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::SSGWGSHertBeatReq, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::SSGSGWHertBeatRsp, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::SSGWGSForwardCSPkg, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::SSGWGSForwardCSPkg, role_id_),
  PROTOBUF_FIELD_OFFSET(::SSGWGSForwardCSPkg, game_id_),
  PROTOBUF_FIELD_OFFSET(::SSGWGSForwardCSPkg, cs_pkg_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::SSGWGSPkgBody, _internal_metadata_),
  ~0u,  // no _extensions_
  PROTOBUF_FIELD_OFFSET(::SSGWGSPkgBody, _oneof_case_[0]),
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  ::PROTOBUF_NAMESPACE_ID::internal::kInvalidFieldOffsetTag,
  ::PROTOBUF_NAMESPACE_ID::internal::kInvalidFieldOffsetTag,
  ::PROTOBUF_NAMESPACE_ID::internal::kInvalidFieldOffsetTag,
  ::PROTOBUF_NAMESPACE_ID::internal::kInvalidFieldOffsetTag,
  PROTOBUF_FIELD_OFFSET(::SSGWGSPkgBody, body_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::SSGWGSInit)},
  { 6, -1, -1, sizeof(::SSGWGSHertBeatReq)},
  { 12, -1, -1, sizeof(::SSGSGWHertBeatRsp)},
  { 18, -1, -1, sizeof(::SSGWGSForwardCSPkg)},
  { 27, -1, -1, sizeof(::SSGWGSPkgBody)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_SSGWGSInit_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_SSGWGSHertBeatReq_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_SSGSGWHertBeatRsp_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_SSGWGSForwardCSPkg_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_SSGWGSPkgBody_default_instance_),
};

const char descriptor_table_protodef_ssgwgs_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\014ssgwgs.proto\032\010cs.proto\"\014\n\nSSGWGSInit\"\023"
  "\n\021SSGWGSHertBeatReq\"\023\n\021SSGSGWHertBeatRsp"
  "\"N\n\022SSGWGSForwardCSPkg\022\017\n\007role_id\030\001 \001(\r\022"
  "\017\n\007game_id\030\002 \001(\r\022\026\n\006cs_pkg\030\003 \001(\0132\006.CSPkg"
  "\"\277\001\n\rSSGWGSPkgBody\022\033\n\004init\030\001 \001(\0132\013.SSGWG"
  "SInitH\000\022,\n\016heart_beat_req\030\002 \001(\0132\022.SSGWGS"
  "HertBeatReqH\000\022,\n\016heart_beat_rsp\030\003 \001(\0132\022."
  "SSGSGWHertBeatRspH\000\022-\n\016forward_cs_pkg\030\004 "
  "\001(\0132\023.SSGWGSForwardCSPkgH\000B\006\n\004bodyb\006prot"
  "o3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_ssgwgs_2eproto_deps[1] = {
  &::descriptor_table_cs_2eproto,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_ssgwgs_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_ssgwgs_2eproto = {
  false, false, 362, descriptor_table_protodef_ssgwgs_2eproto, "ssgwgs.proto", 
  &descriptor_table_ssgwgs_2eproto_once, descriptor_table_ssgwgs_2eproto_deps, 1, 5,
  schemas, file_default_instances, TableStruct_ssgwgs_2eproto::offsets,
  file_level_metadata_ssgwgs_2eproto, file_level_enum_descriptors_ssgwgs_2eproto, file_level_service_descriptors_ssgwgs_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable* descriptor_table_ssgwgs_2eproto_getter() {
  return &descriptor_table_ssgwgs_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_ssgwgs_2eproto(&descriptor_table_ssgwgs_2eproto);

// ===================================================================

class SSGWGSInit::_Internal {
 public:
};

SSGWGSInit::SSGWGSInit(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase(arena, is_message_owned) {
  // @@protoc_insertion_point(arena_constructor:SSGWGSInit)
}
SSGWGSInit::SSGWGSInit(const SSGWGSInit& from)
  : ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:SSGWGSInit)
}





const ::PROTOBUF_NAMESPACE_ID::Message::ClassData SSGWGSInit::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::CopyImpl,
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::MergeImpl,
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*SSGWGSInit::GetClassData() const { return &_class_data_; }







::PROTOBUF_NAMESPACE_ID::Metadata SSGWGSInit::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_ssgwgs_2eproto_getter, &descriptor_table_ssgwgs_2eproto_once,
      file_level_metadata_ssgwgs_2eproto[0]);
}

// ===================================================================

class SSGWGSHertBeatReq::_Internal {
 public:
};

SSGWGSHertBeatReq::SSGWGSHertBeatReq(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase(arena, is_message_owned) {
  // @@protoc_insertion_point(arena_constructor:SSGWGSHertBeatReq)
}
SSGWGSHertBeatReq::SSGWGSHertBeatReq(const SSGWGSHertBeatReq& from)
  : ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:SSGWGSHertBeatReq)
}





const ::PROTOBUF_NAMESPACE_ID::Message::ClassData SSGWGSHertBeatReq::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::CopyImpl,
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::MergeImpl,
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*SSGWGSHertBeatReq::GetClassData() const { return &_class_data_; }







::PROTOBUF_NAMESPACE_ID::Metadata SSGWGSHertBeatReq::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_ssgwgs_2eproto_getter, &descriptor_table_ssgwgs_2eproto_once,
      file_level_metadata_ssgwgs_2eproto[1]);
}

// ===================================================================

class SSGSGWHertBeatRsp::_Internal {
 public:
};

SSGSGWHertBeatRsp::SSGSGWHertBeatRsp(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase(arena, is_message_owned) {
  // @@protoc_insertion_point(arena_constructor:SSGSGWHertBeatRsp)
}
SSGSGWHertBeatRsp::SSGSGWHertBeatRsp(const SSGSGWHertBeatRsp& from)
  : ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:SSGSGWHertBeatRsp)
}





const ::PROTOBUF_NAMESPACE_ID::Message::ClassData SSGSGWHertBeatRsp::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::CopyImpl,
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::MergeImpl,
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*SSGSGWHertBeatRsp::GetClassData() const { return &_class_data_; }







::PROTOBUF_NAMESPACE_ID::Metadata SSGSGWHertBeatRsp::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_ssgwgs_2eproto_getter, &descriptor_table_ssgwgs_2eproto_once,
      file_level_metadata_ssgwgs_2eproto[2]);
}

// ===================================================================

class SSGWGSForwardCSPkg::_Internal {
 public:
  static const ::CSPkg& cs_pkg(const SSGWGSForwardCSPkg* msg);
};

const ::CSPkg&
SSGWGSForwardCSPkg::_Internal::cs_pkg(const SSGWGSForwardCSPkg* msg) {
  return *msg->cs_pkg_;
}
void SSGWGSForwardCSPkg::clear_cs_pkg() {
  if (GetArenaForAllocation() == nullptr && cs_pkg_ != nullptr) {
    delete cs_pkg_;
  }
  cs_pkg_ = nullptr;
}
SSGWGSForwardCSPkg::SSGWGSForwardCSPkg(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor();
  if (!is_message_owned) {
    RegisterArenaDtor(arena);
  }
  // @@protoc_insertion_point(arena_constructor:SSGWGSForwardCSPkg)
}
SSGWGSForwardCSPkg::SSGWGSForwardCSPkg(const SSGWGSForwardCSPkg& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  if (from._internal_has_cs_pkg()) {
    cs_pkg_ = new ::CSPkg(*from.cs_pkg_);
  } else {
    cs_pkg_ = nullptr;
  }
  ::memcpy(&role_id_, &from.role_id_,
    static_cast<size_t>(reinterpret_cast<char*>(&game_id_) -
    reinterpret_cast<char*>(&role_id_)) + sizeof(game_id_));
  // @@protoc_insertion_point(copy_constructor:SSGWGSForwardCSPkg)
}

inline void SSGWGSForwardCSPkg::SharedCtor() {
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&cs_pkg_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&game_id_) -
    reinterpret_cast<char*>(&cs_pkg_)) + sizeof(game_id_));
}

SSGWGSForwardCSPkg::~SSGWGSForwardCSPkg() {
  // @@protoc_insertion_point(destructor:SSGWGSForwardCSPkg)
  if (GetArenaForAllocation() != nullptr) return;
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

inline void SSGWGSForwardCSPkg::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  if (this != internal_default_instance()) delete cs_pkg_;
}

void SSGWGSForwardCSPkg::ArenaDtor(void* object) {
  SSGWGSForwardCSPkg* _this = reinterpret_cast< SSGWGSForwardCSPkg* >(object);
  (void)_this;
}
void SSGWGSForwardCSPkg::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void SSGWGSForwardCSPkg::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void SSGWGSForwardCSPkg::Clear() {
// @@protoc_insertion_point(message_clear_start:SSGWGSForwardCSPkg)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  if (GetArenaForAllocation() == nullptr && cs_pkg_ != nullptr) {
    delete cs_pkg_;
  }
  cs_pkg_ = nullptr;
  ::memset(&role_id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&game_id_) -
      reinterpret_cast<char*>(&role_id_)) + sizeof(game_id_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* SSGWGSForwardCSPkg::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // uint32 role_id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          role_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // uint32 game_id = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          game_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // .CSPkg cs_pkg = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 26)) {
          ptr = ctx->ParseMessage(_internal_mutable_cs_pkg(), ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* SSGWGSForwardCSPkg::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:SSGWGSForwardCSPkg)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // uint32 role_id = 1;
  if (this->_internal_role_id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_role_id(), target);
  }

  // uint32 game_id = 2;
  if (this->_internal_game_id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_game_id(), target);
  }

  // .CSPkg cs_pkg = 3;
  if (this->_internal_has_cs_pkg()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        3, _Internal::cs_pkg(this), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:SSGWGSForwardCSPkg)
  return target;
}

size_t SSGWGSForwardCSPkg::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:SSGWGSForwardCSPkg)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // .CSPkg cs_pkg = 3;
  if (this->_internal_has_cs_pkg()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *cs_pkg_);
  }

  // uint32 role_id = 1;
  if (this->_internal_role_id() != 0) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32SizePlusOne(this->_internal_role_id());
  }

  // uint32 game_id = 2;
  if (this->_internal_game_id() != 0) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32SizePlusOne(this->_internal_game_id());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData SSGWGSForwardCSPkg::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSizeCheck,
    SSGWGSForwardCSPkg::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*SSGWGSForwardCSPkg::GetClassData() const { return &_class_data_; }

void SSGWGSForwardCSPkg::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to,
                      const ::PROTOBUF_NAMESPACE_ID::Message& from) {
  static_cast<SSGWGSForwardCSPkg *>(to)->MergeFrom(
      static_cast<const SSGWGSForwardCSPkg &>(from));
}


void SSGWGSForwardCSPkg::MergeFrom(const SSGWGSForwardCSPkg& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:SSGWGSForwardCSPkg)
  GOOGLE_DCHECK_NE(&from, this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_cs_pkg()) {
    _internal_mutable_cs_pkg()->::CSPkg::MergeFrom(from._internal_cs_pkg());
  }
  if (from._internal_role_id() != 0) {
    _internal_set_role_id(from._internal_role_id());
  }
  if (from._internal_game_id() != 0) {
    _internal_set_game_id(from._internal_game_id());
  }
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void SSGWGSForwardCSPkg::CopyFrom(const SSGWGSForwardCSPkg& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:SSGWGSForwardCSPkg)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SSGWGSForwardCSPkg::IsInitialized() const {
  return true;
}

void SSGWGSForwardCSPkg::InternalSwap(SSGWGSForwardCSPkg* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(SSGWGSForwardCSPkg, game_id_)
      + sizeof(SSGWGSForwardCSPkg::game_id_)
      - PROTOBUF_FIELD_OFFSET(SSGWGSForwardCSPkg, cs_pkg_)>(
          reinterpret_cast<char*>(&cs_pkg_),
          reinterpret_cast<char*>(&other->cs_pkg_));
}

::PROTOBUF_NAMESPACE_ID::Metadata SSGWGSForwardCSPkg::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_ssgwgs_2eproto_getter, &descriptor_table_ssgwgs_2eproto_once,
      file_level_metadata_ssgwgs_2eproto[3]);
}

// ===================================================================

class SSGWGSPkgBody::_Internal {
 public:
  static const ::SSGWGSInit& init(const SSGWGSPkgBody* msg);
  static const ::SSGWGSHertBeatReq& heart_beat_req(const SSGWGSPkgBody* msg);
  static const ::SSGSGWHertBeatRsp& heart_beat_rsp(const SSGWGSPkgBody* msg);
  static const ::SSGWGSForwardCSPkg& forward_cs_pkg(const SSGWGSPkgBody* msg);
};

const ::SSGWGSInit&
SSGWGSPkgBody::_Internal::init(const SSGWGSPkgBody* msg) {
  return *msg->body_.init_;
}
const ::SSGWGSHertBeatReq&
SSGWGSPkgBody::_Internal::heart_beat_req(const SSGWGSPkgBody* msg) {
  return *msg->body_.heart_beat_req_;
}
const ::SSGSGWHertBeatRsp&
SSGWGSPkgBody::_Internal::heart_beat_rsp(const SSGWGSPkgBody* msg) {
  return *msg->body_.heart_beat_rsp_;
}
const ::SSGWGSForwardCSPkg&
SSGWGSPkgBody::_Internal::forward_cs_pkg(const SSGWGSPkgBody* msg) {
  return *msg->body_.forward_cs_pkg_;
}
void SSGWGSPkgBody::set_allocated_init(::SSGWGSInit* init) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  clear_body();
  if (init) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
      ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper<::SSGWGSInit>::GetOwningArena(init);
    if (message_arena != submessage_arena) {
      init = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, init, submessage_arena);
    }
    set_has_init();
    body_.init_ = init;
  }
  // @@protoc_insertion_point(field_set_allocated:SSGWGSPkgBody.init)
}
void SSGWGSPkgBody::set_allocated_heart_beat_req(::SSGWGSHertBeatReq* heart_beat_req) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  clear_body();
  if (heart_beat_req) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
      ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper<::SSGWGSHertBeatReq>::GetOwningArena(heart_beat_req);
    if (message_arena != submessage_arena) {
      heart_beat_req = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, heart_beat_req, submessage_arena);
    }
    set_has_heart_beat_req();
    body_.heart_beat_req_ = heart_beat_req;
  }
  // @@protoc_insertion_point(field_set_allocated:SSGWGSPkgBody.heart_beat_req)
}
void SSGWGSPkgBody::set_allocated_heart_beat_rsp(::SSGSGWHertBeatRsp* heart_beat_rsp) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  clear_body();
  if (heart_beat_rsp) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
      ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper<::SSGSGWHertBeatRsp>::GetOwningArena(heart_beat_rsp);
    if (message_arena != submessage_arena) {
      heart_beat_rsp = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, heart_beat_rsp, submessage_arena);
    }
    set_has_heart_beat_rsp();
    body_.heart_beat_rsp_ = heart_beat_rsp;
  }
  // @@protoc_insertion_point(field_set_allocated:SSGWGSPkgBody.heart_beat_rsp)
}
void SSGWGSPkgBody::set_allocated_forward_cs_pkg(::SSGWGSForwardCSPkg* forward_cs_pkg) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  clear_body();
  if (forward_cs_pkg) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
      ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper<::SSGWGSForwardCSPkg>::GetOwningArena(forward_cs_pkg);
    if (message_arena != submessage_arena) {
      forward_cs_pkg = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, forward_cs_pkg, submessage_arena);
    }
    set_has_forward_cs_pkg();
    body_.forward_cs_pkg_ = forward_cs_pkg;
  }
  // @@protoc_insertion_point(field_set_allocated:SSGWGSPkgBody.forward_cs_pkg)
}
SSGWGSPkgBody::SSGWGSPkgBody(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor();
  if (!is_message_owned) {
    RegisterArenaDtor(arena);
  }
  // @@protoc_insertion_point(arena_constructor:SSGWGSPkgBody)
}
SSGWGSPkgBody::SSGWGSPkgBody(const SSGWGSPkgBody& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  clear_has_body();
  switch (from.body_case()) {
    case kInit: {
      _internal_mutable_init()->::SSGWGSInit::MergeFrom(from._internal_init());
      break;
    }
    case kHeartBeatReq: {
      _internal_mutable_heart_beat_req()->::SSGWGSHertBeatReq::MergeFrom(from._internal_heart_beat_req());
      break;
    }
    case kHeartBeatRsp: {
      _internal_mutable_heart_beat_rsp()->::SSGSGWHertBeatRsp::MergeFrom(from._internal_heart_beat_rsp());
      break;
    }
    case kForwardCsPkg: {
      _internal_mutable_forward_cs_pkg()->::SSGWGSForwardCSPkg::MergeFrom(from._internal_forward_cs_pkg());
      break;
    }
    case BODY_NOT_SET: {
      break;
    }
  }
  // @@protoc_insertion_point(copy_constructor:SSGWGSPkgBody)
}

inline void SSGWGSPkgBody::SharedCtor() {
clear_has_body();
}

SSGWGSPkgBody::~SSGWGSPkgBody() {
  // @@protoc_insertion_point(destructor:SSGWGSPkgBody)
  if (GetArenaForAllocation() != nullptr) return;
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

inline void SSGWGSPkgBody::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  if (has_body()) {
    clear_body();
  }
}

void SSGWGSPkgBody::ArenaDtor(void* object) {
  SSGWGSPkgBody* _this = reinterpret_cast< SSGWGSPkgBody* >(object);
  (void)_this;
}
void SSGWGSPkgBody::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void SSGWGSPkgBody::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void SSGWGSPkgBody::clear_body() {
// @@protoc_insertion_point(one_of_clear_start:SSGWGSPkgBody)
  switch (body_case()) {
    case kInit: {
      if (GetArenaForAllocation() == nullptr) {
        delete body_.init_;
      }
      break;
    }
    case kHeartBeatReq: {
      if (GetArenaForAllocation() == nullptr) {
        delete body_.heart_beat_req_;
      }
      break;
    }
    case kHeartBeatRsp: {
      if (GetArenaForAllocation() == nullptr) {
        delete body_.heart_beat_rsp_;
      }
      break;
    }
    case kForwardCsPkg: {
      if (GetArenaForAllocation() == nullptr) {
        delete body_.forward_cs_pkg_;
      }
      break;
    }
    case BODY_NOT_SET: {
      break;
    }
  }
  _oneof_case_[0] = BODY_NOT_SET;
}


void SSGWGSPkgBody::Clear() {
// @@protoc_insertion_point(message_clear_start:SSGWGSPkgBody)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  clear_body();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* SSGWGSPkgBody::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // .SSGWGSInit init = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          ptr = ctx->ParseMessage(_internal_mutable_init(), ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // .SSGWGSHertBeatReq heart_beat_req = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 18)) {
          ptr = ctx->ParseMessage(_internal_mutable_heart_beat_req(), ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // .SSGSGWHertBeatRsp heart_beat_rsp = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 26)) {
          ptr = ctx->ParseMessage(_internal_mutable_heart_beat_rsp(), ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // .SSGWGSForwardCSPkg forward_cs_pkg = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 34)) {
          ptr = ctx->ParseMessage(_internal_mutable_forward_cs_pkg(), ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* SSGWGSPkgBody::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:SSGWGSPkgBody)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // .SSGWGSInit init = 1;
  if (_internal_has_init()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        1, _Internal::init(this), target, stream);
  }

  // .SSGWGSHertBeatReq heart_beat_req = 2;
  if (_internal_has_heart_beat_req()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        2, _Internal::heart_beat_req(this), target, stream);
  }

  // .SSGSGWHertBeatRsp heart_beat_rsp = 3;
  if (_internal_has_heart_beat_rsp()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        3, _Internal::heart_beat_rsp(this), target, stream);
  }

  // .SSGWGSForwardCSPkg forward_cs_pkg = 4;
  if (_internal_has_forward_cs_pkg()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        4, _Internal::forward_cs_pkg(this), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:SSGWGSPkgBody)
  return target;
}

size_t SSGWGSPkgBody::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:SSGWGSPkgBody)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  switch (body_case()) {
    // .SSGWGSInit init = 1;
    case kInit: {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
          *body_.init_);
      break;
    }
    // .SSGWGSHertBeatReq heart_beat_req = 2;
    case kHeartBeatReq: {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
          *body_.heart_beat_req_);
      break;
    }
    // .SSGSGWHertBeatRsp heart_beat_rsp = 3;
    case kHeartBeatRsp: {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
          *body_.heart_beat_rsp_);
      break;
    }
    // .SSGWGSForwardCSPkg forward_cs_pkg = 4;
    case kForwardCsPkg: {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
          *body_.forward_cs_pkg_);
      break;
    }
    case BODY_NOT_SET: {
      break;
    }
  }
  return MaybeComputeUnknownFieldsSize(total_size, &_cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData SSGWGSPkgBody::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSizeCheck,
    SSGWGSPkgBody::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*SSGWGSPkgBody::GetClassData() const { return &_class_data_; }

void SSGWGSPkgBody::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to,
                      const ::PROTOBUF_NAMESPACE_ID::Message& from) {
  static_cast<SSGWGSPkgBody *>(to)->MergeFrom(
      static_cast<const SSGWGSPkgBody &>(from));
}


void SSGWGSPkgBody::MergeFrom(const SSGWGSPkgBody& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:SSGWGSPkgBody)
  GOOGLE_DCHECK_NE(&from, this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  switch (from.body_case()) {
    case kInit: {
      _internal_mutable_init()->::SSGWGSInit::MergeFrom(from._internal_init());
      break;
    }
    case kHeartBeatReq: {
      _internal_mutable_heart_beat_req()->::SSGWGSHertBeatReq::MergeFrom(from._internal_heart_beat_req());
      break;
    }
    case kHeartBeatRsp: {
      _internal_mutable_heart_beat_rsp()->::SSGSGWHertBeatRsp::MergeFrom(from._internal_heart_beat_rsp());
      break;
    }
    case kForwardCsPkg: {
      _internal_mutable_forward_cs_pkg()->::SSGWGSForwardCSPkg::MergeFrom(from._internal_forward_cs_pkg());
      break;
    }
    case BODY_NOT_SET: {
      break;
    }
  }
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void SSGWGSPkgBody::CopyFrom(const SSGWGSPkgBody& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:SSGWGSPkgBody)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SSGWGSPkgBody::IsInitialized() const {
  return true;
}

void SSGWGSPkgBody::InternalSwap(SSGWGSPkgBody* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(body_, other->body_);
  swap(_oneof_case_[0], other->_oneof_case_[0]);
}

::PROTOBUF_NAMESPACE_ID::Metadata SSGWGSPkgBody::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_ssgwgs_2eproto_getter, &descriptor_table_ssgwgs_2eproto_once,
      file_level_metadata_ssgwgs_2eproto[4]);
}

// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::SSGWGSInit* Arena::CreateMaybeMessage< ::SSGWGSInit >(Arena* arena) {
  return Arena::CreateMessageInternal< ::SSGWGSInit >(arena);
}
template<> PROTOBUF_NOINLINE ::SSGWGSHertBeatReq* Arena::CreateMaybeMessage< ::SSGWGSHertBeatReq >(Arena* arena) {
  return Arena::CreateMessageInternal< ::SSGWGSHertBeatReq >(arena);
}
template<> PROTOBUF_NOINLINE ::SSGSGWHertBeatRsp* Arena::CreateMaybeMessage< ::SSGSGWHertBeatRsp >(Arena* arena) {
  return Arena::CreateMessageInternal< ::SSGSGWHertBeatRsp >(arena);
}
template<> PROTOBUF_NOINLINE ::SSGWGSForwardCSPkg* Arena::CreateMaybeMessage< ::SSGWGSForwardCSPkg >(Arena* arena) {
  return Arena::CreateMessageInternal< ::SSGWGSForwardCSPkg >(arena);
}
template<> PROTOBUF_NOINLINE ::SSGWGSPkgBody* Arena::CreateMaybeMessage< ::SSGWGSPkgBody >(Arena* arena) {
  return Arena::CreateMessageInternal< ::SSGWGSPkgBody >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
