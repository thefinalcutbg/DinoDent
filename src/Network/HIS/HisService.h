#pragma once
#include "Network/AbstractReplyHandler.h"

#include <string_view>

struct Patient;

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

	std::string bind(const std::string& name, std::string value);
	std::string bind(const std::string& name, int value, bool ommitZero = true);
	std::string bind(const std::string& name, bool value);
	std::string bind(const std::string& name, double value);
	std::string bind(const std::string& name, const char* value);

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

