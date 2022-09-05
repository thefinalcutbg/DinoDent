#pragma once
#include "GetNumenclature.h"

class UpdateMedications : private GetNumenclature
{

	virtual void parseReply(const std::string& reply) override;

public:
	void update();
};

