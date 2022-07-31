#pragma once
#include "Network/Common/AbstractReplyHandler.h"

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

	enum class SOAPAction { View, Files, Diagnosis };
	bool sendRequest(const std::string& query, SOAPAction header);

};

