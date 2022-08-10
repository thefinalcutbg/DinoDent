#pragma once
#include "HisService.h"
#include <functional>

class OpenDentalExam final : private HisService
{
	std::function<void(const std::string& nrn)> m_callback;
protected:
	void parseReply(const std::string& reply) override {};

public:
	OpenDentalExam() : HisService("C001", "/v1/nomenclatures/all/get") {};
	//returns the new NRN
	bool sendRequest(std::function<void(const std::string&)> callback);
};