#pragma once

#include <functional>
#include <optional>

#include "Network/AbstractReplyHandler.h"
#include "Model/Patient.h"
#include "Model/Pention.h"

class NssiPentionService final : protected AbstractReplyHandler
{
	std::function<void(const std::optional<Pention>&)> m_callback;
	void parseReply(const std::string& reply) override;
public:
	bool sendRequest(
		const Patient& p,
		std::function<void(const std::optional<Pention>&)> callback,
		bool showDialogs = true
	);

};