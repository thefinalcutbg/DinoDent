#pragma once
#include "PisService.h"
#include <functional>

class GetHirbnoService final: private PisService
{
	std::function<void(const std::string&)> m_callback;
	void parseReply(const std::string& reply) override;
public:

	void sendRequest(
		int personType,
		const std::string& id,
		std::function<void(const std::string&)> callback
	);
};

