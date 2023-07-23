#pragma once
#include "Network/AbstractReplyHandler.h"

struct Patient;

class PisService : protected AbstractReplyHandler {

	void parseReply(const std::string& reply) final;

protected:

	std::string getPersonIdTag(const Patient& p);
	enum class SOAPAction { View, Files, Diagnosis };
	bool sendRequest(const std::string& query, SOAPAction header);
	virtual void processPISReply(const std::string& reply) = 0;
public:

	bool awaitingReply() {
		return awaiting_reply;
	}
};

