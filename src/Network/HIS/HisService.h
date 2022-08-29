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
	std::string requester();

	HisService(const std::string& messageType, const std::string& servPath)
		:
		messageType{ messageType },
		servicePath{ servPath }
	{}
public:
	bool sendRequestToHis(const std::string& query);

};

