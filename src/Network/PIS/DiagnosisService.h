#pragma once
#include "PisService.h"

#include <functional>
#include <optional>
#include <vector>

#include "Model/MedicalStatuses.h"

class DiagnosisService final: private PisService
{
	std::function<void(
		const std::vector<std::string>& current, 
		const std::vector<std::string>& past
	)> m_callback;
	void processPISReply(const std::string& reply) override;
public:

	bool sendRequest(
		const Patient& p,
		decltype(m_callback) callback
	);

	bool awaitingReply() { return awaiting_reply; }

};
