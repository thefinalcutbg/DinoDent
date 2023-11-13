#pragma once
#include "AbstractValidator.h"

class BridgeRangeValidator : public Validator
{
public:
	bool allowSingleRange = false;
	bool validateInput(int begin, int end) override;
};

class SurfaceValidator : public Validator
{
public:
	bool validateInput(const std::array<bool, 6>& surfaces) override;
};
