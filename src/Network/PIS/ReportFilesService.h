#pragma once
#pragma once
#include "PisService.h"

#include <functional>
#include <optional>
#include "Model/Dental/ReportFile.h"

class ReportFilesService final : private PisService
{

	std::function<void(const std::vector<ReportFile>&)> m_callback;
	void parseReply(const std::string& reply) override;
public:

	void sendRequest(
		const std::string& rziCode,
		std::function<void(const std::vector<ReportFile>&)> callback
	);

};
