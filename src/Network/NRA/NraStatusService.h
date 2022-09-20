#pragma once
#include "Network/AbstractReplyHandler.h"
#include <TinyXML/tinyxml.h>
#include "Model/InsuranceStatus.h"
#include <functional>
#include <optional>
#include "Model/Date.h"
struct Patient;


class NraStatusService final : protected AbstractReplyHandler
{
	std::function<void(const std::optional<InsuranceStatus>&)> m_callback;
	void parseReply(const std::string& reply) override;
public:
	bool sendRequest(
		const Patient& p, 
		std::function<void(const std::optional<InsuranceStatus>&)> callback,
		const Date& date = Date::currentDate()
	);

};

