#pragma once
#include <string>

class ReplyHandler;

namespace PISServ
{

	std::string activeHIRBNo(const std::string& id, int personType);
	std::string parseHIRBNoReply(const std::string& reply);
	void sendRequest(const std::string& soapBody, ReplyHandler* handler);
	
}

