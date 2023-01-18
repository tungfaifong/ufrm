// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "common.h"

#include <signal.h>

#include "interfaces/logger_interface.h"

#include "protocol/cs.pb.h"
#include "protocol/ss.pb.h"

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

void ConvertMysqlpp2PBVariant(const mysqlpp::String & mysql_v, Variant & pb_v)
{
	auto type = mysql_v.type();
	if (type == typeid(bool) || type == typeid(mysqlpp::sql_bool) || type == typeid(mysqlpp::sql_bool_null))
	{
		if(mysql_v.is_null())
		{
			pb_v.set_bool_(false);
		}
		else
		{
			pb_v.set_bool_((bool)mysql_v);
		}
	}
	else if(type == typeid(uint32_t) || type == typeid(mysqlpp::sql_int_unsigned_null))
	{
		if(mysql_v.is_null())
		{
			pb_v.set_uint32_(0);
		}
		else
		{
			pb_v.set_uint32_((uint32_t)mysql_v);
		}
	}
	else if(type == typeid(int32_t) || type == typeid(mysqlpp::sql_int_null))
	{
		if(mysql_v.is_null())
		{
			pb_v.set_int32_(0);
		}
		else
		{
			pb_v.set_int32_((int32_t)mysql_v);
		}
	}
	else if(type == typeid(uint64_t) || type == typeid(mysqlpp::sql_bigint_unsigned_null))
	{
		if(mysql_v.is_null())
		{
			pb_v.set_uint64_(0);
		}
		else
		{
			pb_v.set_uint64_((uint64_t)mysql_v);
		}
	}
	else if(type == typeid(int64_t) || type == typeid(mysqlpp::sql_bigint_null))
	{
		if(mysql_v.is_null())
		{
			pb_v.set_int64_(0);
		}
		else
		{
			pb_v.set_int64_((int64_t)mysql_v);
		}
	}
	else if(type == typeid(float) || type == typeid(mysqlpp::sql_float_null))
	{
		if(mysql_v.is_null())
		{
			pb_v.set_float_(0);
		}
		else
		{
			pb_v.set_float_((float)mysql_v);
		}
	}
	else if(type == typeid(double) || type == typeid(mysqlpp::sql_double_null))
	{
		if(mysql_v.is_null())
		{
			pb_v.set_double_(0);
		}
		else
		{
			pb_v.set_double_((double)mysql_v);
		}
	}
	else if(type == typeid(std::string) || type == typeid(mysqlpp::sql_blob) ||
			type == typeid(mysqlpp::sql_text_null) || type == typeid(mysqlpp::sql_blob_null))
	{
		if(mysql_v.is_null())
		{
			pb_v.set_string_("");
		}
		else
		{
			pb_v.set_string_((std::string)mysql_v);
		}
	}
}


void TraceMsg(const std::string & prefix, const google::protobuf::Message * pkg)
{
	if(logger::Level() > LoggerUnit::LEVEL::TRACE)
	{
		return;
	}

	if(pkg->GetTypeName() == "CSPkgHead")
	{
		CSPkgHead * head = (CSPkgHead *)pkg;
		if (head->id() == CSID_HEART_BEAT_REQ ||
			head->id() == SCID_HEART_BEAT_RSP)
		{
			return;
		}
	}
	else if(pkg->GetTypeName() == "SSPkgHead")
	{
		SSPkgHead * head = (SSPkgHead *)pkg;
		if (head->id() == SSID_LC_LS_HEART_BEAT_REQ ||
			head->id() == SSID_LS_LC_HEART_BEAT_RSP ||
			head->id() == SSID_GW_GS_HEART_BEAT_REQ ||
			head->id() == SSID_GS_GW_HEART_BEAT_RSP ||
			head->id() == SSID_PC_PX_HEART_BEAT_REQ ||
			head->id() == SSID_PX_PC_HEART_BEAT_RSP)
		{
			return;
		}
	}
	else if(pkg->GetTypeName() == "CSHeartBeatReq" ||
			pkg->GetTypeName() == "SCHeartBeatRsp" ||
			pkg->GetTypeName() == "SSLCLSHeartBeatReq" ||
			pkg->GetTypeName() == "SSLSLCHeartBeatRsp" ||
			pkg->GetTypeName() == "SSGWGSHertBeatReq" ||
			pkg->GetTypeName() == "SSGSGWHertBeatRsp" ||
			pkg->GetTypeName() == "SSPCPXHeartBeatReq" ||
			pkg->GetTypeName() == "SSPXPCHeartBeatRsp")
	{
		return;
	}

	LOGGER_TRACE("{} msg pkg:\n{} {{\n{}}}", prefix, pkg->GetTypeName(), pkg->DebugString());
}

void SignalHandler(int signo)
{
	switch(signo)
	{
		case SIGILL:
		case SIGTRAP:
		case SIGABRT:
		case SIGBUS:
		case SIGFPE:
		case SIGSEGV:
		case SIGSYS:
		{
			signal(signo, SIG_DFL);
			logger::OnAbort();
		}
		break;
		case SIGUSR1:
			UnitManager::Instance()->SetExit(true);
			break;
		default:
			break;
	}
}

void SignalInit()
{
	signal(SIGILL, SignalHandler);
	signal(SIGTRAP, SignalHandler);
	signal(SIGABRT, SignalHandler);
	signal(SIGBUS, SignalHandler);
	signal(SIGFPE, SignalHandler);
	signal(SIGSEGV, SignalHandler);
	signal(SIGSEGV, SignalHandler);
	signal(SIGUSR1, SignalHandler);
}
