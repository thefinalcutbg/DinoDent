#pragma once
#pragma once
#include "GetNumenclature.h"

class UpdateMkb : public GetNumenclature
{
	bool parseNumenclature(const std::string& reply) override;

public:
	void update();
};
