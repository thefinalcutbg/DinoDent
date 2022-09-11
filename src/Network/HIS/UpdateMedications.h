#pragma once
#include "GetNumenclature.h"

class UpdateMedications : private GetNumenclature
{
	 void parseReply(const std::string& reply) override;

public:
	void update();
};

