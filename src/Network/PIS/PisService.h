#pragma once
#include "Network/AbstractReplyHandler.h"

class PisService : protected AbstractReplyHandler {

	void parseReply(const std::string& reply) final;

protected:

	inline static 	const char* personTypeArr[6]
	{
		"",
		"egn",
		"lnch",
		"nra",
		"ehc",
		"con"
	};

	enum class SOAPAction { View, Files, Diagnosis };
	bool sendRequest(const std::string& query, SOAPAction header);
	virtual void processPISReply(const std::string& reply) = 0;
public:

	bool awaitingReply() {
		return awaiting_reply;
	}
};

