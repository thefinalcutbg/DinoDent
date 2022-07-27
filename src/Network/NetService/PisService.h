#pragma once
#include "AbstractReplyHandler.h"

class PisService : protected AbstractReplyHandler {

protected:

	inline static 	const char* personTypeArr[5]
	{
		"",
		"egn",
		"lnch",
		"",
		"",
	};

	enum class SOAPAction { View, Files };
	bool sendRequest(const std::string& query, SOAPAction header);

};

