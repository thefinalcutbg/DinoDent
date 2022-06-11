#pragma once
#include <string>
#include <vector>
#include "Model/Date.h"
class AbstractReplyHandler;

namespace SOAP
{
	std::string activeHIRBNo(const std::string& id, int personType);
	std::string dentalActivities(const std::string& id, int personType);
}

namespace PIS
{
	//returns true if the request was passed to the network
	bool sendRequest(const std::string& soapBody, AbstractReplyHandler& handler);
	
}

