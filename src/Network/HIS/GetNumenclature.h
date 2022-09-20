#pragma once
#include "HisService.h"

class GetNumenclature : private HisService
{

protected:
	//virtual void parseReply(const std::string& reply) override;

public:
	GetNumenclature();
	bool sendRequest(int numenclature);
};
