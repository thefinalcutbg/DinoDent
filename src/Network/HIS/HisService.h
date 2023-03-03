#pragma once
#include "Network/AbstractReplyHandler.h"

#include <string_view>

class ProcedureContainer;
class ToothContainer;
struct Patient;
class Tooth;
//constexpr const char* hisUrl = "https://api.his.bg/";
constexpr const char* hisUrl = "https://ptest-api.his.bg/";

class HisService : protected AbstractReplyHandler
{

private:

	const std::string buildMessage(const std::string& query);
	const std::string signMessage(const std::string& message);

	const std::string messageType;
	const std::string servicePath;

protected:

	std::string subject(const Patient& p);
	std::string requester(bool includeNhifCode = false);
	std::string performer();
	std::string getStatus(const ToothContainer& teeth);
	std::string getProcedures(const ProcedureContainer& procedures, const ToothContainer& teeth);
	std::string getToothStatus(const Tooth& tooth, bool includeTimestamp = true);

	std::string bind(const std::string& name, const std::string& value, bool isUserInput = false);
	std::string bind(const std::string& name, int value, bool ommitZero = true);
	std::string bind(const std::string& name, bool value);
	std::string bind(const std::string& name, double value);
	std::string bind(const std::string& name, const char* value, bool isUserInput = false);

	std::string getErrors(const std::string& reply);

	HisService(const std::string& messageType, const std::string& servPath)
		:
		messageType{ messageType },
		servicePath{ servPath }
	{}
public:
	bool sendRequestToHis(const std::string& contents);
	bool sendRequestToHisNoAuth(const std::string& contents);

};

