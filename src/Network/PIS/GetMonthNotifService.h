#pragma once
#include "PisService.h"
#include <functional>

class GetMonthNotifService final :private PisService
{
	std::function<void(const std::string&)> m_callback;
	void parseReply(const std::string& reply) override;
public:
	void sendRequest(
		const std::string& rziCode,
		const std::string& notifHash,
		std::function<void(const std::string&)> callback
	);
};

