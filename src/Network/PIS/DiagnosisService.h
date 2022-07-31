#pragma once
#include "PisService.h"

#include <functional>
#include <optional>
#include "Model/Procedure/Procedure.h"

class DiagnosisService final: private PisService
{
	std::function<void(const std::string&)> m_callback;
	void parseReply(const std::string& reply) override;
public:

	bool sendRequest(
		int personType,
		const std::string& patientId,
		std::function<void(const std::string&)> callback
	);

};
