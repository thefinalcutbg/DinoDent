#pragma once
#include "Network/Common/AbstractReplyHandler.h"

class HisService : protected AbstractReplyHandler
{
	virtual void parseReply(const std::string& reply) override;

	//returns empty string if signing has failed
	std::string signMessage(const std::string& message);
public:
	bool sendRequest(const std::string& query);


};

