// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "common.h"

void ConvertVariant2PBVariant(const variant_t & v, Variant & pb_v)
{
	switch((VARIANTIDX)v.index())
	{
	case VARIANTIDX::BOOL:
		{
			pb_v.set_bool_(std::get<bool>(v));
		}
		break;
	case VARIANTIDX::UINT32:
		{
			pb_v.set_uint32_(std::get<uint32_t>(v));
		}
		break;
	case VARIANTIDX::INT32:
		{
			pb_v.set_int32_(std::get<int32_t>(v));
		}
		break;
	case VARIANTIDX::UINT64:
		{
			pb_v.set_uint64_(std::get<uint64_t>(v));
		}
		break;
	case VARIANTIDX::INT64:
		{
			pb_v.set_int64_(std::get<int64_t>(v));
		}
		break;
	case VARIANTIDX::FLOAT:
		{
			pb_v.set_float_(std::get<float>(v));
		}
		break;
	case VARIANTIDX::DOUBLE:
		{
			pb_v.set_double_(std::get<double>(v));
		}
		break;
	case VARIANTIDX::STRING:
		{
			pb_v.set_string_(std::get<std::string>(v));
		}
		break;
	default:
		break;
	}
}

void ConvertPBVariant2Variant(const Variant & pb_v, variant_t & v)
{
	switch(pb_v.value_case())
	{
	case Variant::ValueCase::kBool:
		{
			v = pb_v.bool_();
		}
		break;
	case Variant::ValueCase::kInt32:
		{
			v = pb_v.int32_();
		}
		break;
	case Variant::ValueCase::kUint32:
		{
			v = pb_v.uint32_();
		}
		break;
	case Variant::ValueCase::kInt64:
		{
			v = pb_v.int64_();
		}
		break;
	case Variant::ValueCase::kUint64:
		{
			v = pb_v.uint64_();
		}
		break;
	case Variant::ValueCase::kFloat:
		{
			v = pb_v.float_();
		}
		break;
	case Variant::ValueCase::kDouble:
		{
			v = pb_v.double_();
		}
		break;
	case Variant::ValueCase::kString:
		{
			v = pb_v.string_();
		}
		break;
	default:
		break;
	}
}
