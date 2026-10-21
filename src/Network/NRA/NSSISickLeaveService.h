#pragma once
#include "Network/AbstractReplyHandler.h"
#include "Model/SickLeave.h"
#include "Model/Patient.h"

class NSSISickLeaveService : private AbstractReplyHandler
{
	void parseReply(const std::string& reply);

public:
	bool sendRequest(const Patient& p, const SickLeave& sickLeave, const std::string& nrn, const std::string& deleteReason = std::string());
};