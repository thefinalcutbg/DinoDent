#pragma once

#include <functional>
#include <optional>

#include "Network/AbstractReplyHandler.h"
#include "Model/Patient.h"


class NssiPentionService final : protected AbstractReplyHandler
{
	void parseReply(const std::string& reply) override;
public:
	bool sendRequest(const Patient& p);

};