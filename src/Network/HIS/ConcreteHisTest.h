#pragma once
#include "HisService.h"

class ConcreteHisTest : private HisService
{

protected:
	virtual void parseReply(const std::string& reply) override;

public:
	ConcreteHisTest() : HisService("C001", "/v1/nomenclatures/all/get") {};
	bool sendRequest(int numenclature);
};
