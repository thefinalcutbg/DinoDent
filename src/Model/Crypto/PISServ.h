#pragma once
#include <string>

namespace PISServ
{

	std::string activeHIRBNo(const std::string& id, int personType);
	
	void sendRequest(const std::string& soapBody);
	
}

