#pragma once

#pragma once
#pragma once
#include "PisService.h"

#include <functional>
#include <optional>
#include "Model/Dental/ReportFile.h"
#include <stack>

class GetAmbHashes final : private PisService
{
	std::function<void(std::stack<std::string>& hashes)> m_callback;

	void processPISReply(const std::string& reply) override;
public:

	bool sendRequest(
		const std::string& docLPK,
		int year,
		decltype(m_callback) callback
	);
};


class GetFileService final : private PisService
{

	std::function<void(const std::string&)> m_callback;
	void processPISReply(const std::string& reply) override;
public:

	bool sendRequest(
		const std::string& fileHash,
		const std::string& docLPK,
		std::function<void(const std::string& file)> callback
	);
};
