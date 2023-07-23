#pragma once
#include "PisService.h"
#include <functional>

class GetHirbnoService final: private PisService
{
	std::function<void(const std::string&)> m_callback;
	void processPISReply(const std::string& reply) override;
public:

	void sendRequest(
		const Patient& p,
		std::function<void(const std::string&)> callback
	);
};

