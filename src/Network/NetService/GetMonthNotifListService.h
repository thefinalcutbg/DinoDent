#pragma once
#include "PisService.h"
#include "Model/Financial/MonthNotifRow.h"
#include <vector>
#include <optional>
#include <functional>

class GetMonthNotifListService : public PisService
{

	std::function<void(const std::optional<std::vector<MonthNotifRow>>&)> m_callback;
	void parseReply(const std::string& reply) override;

public:
	void sendRequest(
		const std::string& rziCode,
		std::function<void(const std::optional<std::vector<MonthNotifRow>>&)> callback
	);
};

