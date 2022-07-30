#pragma once
#include "Network/Common/AbstractReplyHandler.h"

class HisService : protected AbstractReplyHandler
{
	virtual void parseReply(const std::string& reply) override;
	virtual const std::string& getServicePath() = 0;
	//returns empty string if signing has failed
	std::string signMessage(const std::string& message);

public:
	bool sendRequest(const std::string& query);


};

