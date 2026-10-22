#pragma once
#include "Network/AbstractReplyHandler.h"
#include "Model/SickLeave.h"
#include "Model/Patient.h"
#include <functional>

class NSSISickLeaveService : private AbstractReplyHandler
{
	void parseReply(const std::string& reply);

	std::function<void(long long)> m_callback;

	std::string tag(const std::string& name, const std::string& value);
	std::string tag(const std::string& name, long long value);
	std::string regimenXml(int regimeType, int days);

public:
	//provide delete reason for cancel
	bool sendRequest(const Patient& p, const SickLeave& sickLeave, const std::string& nrn, const std::string& deleteReason, decltype(m_callback) callcback);
};