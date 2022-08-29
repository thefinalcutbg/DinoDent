#pragma once
#include"CommonValidators.h"

class NzokContractValidator : public Validator
{
	ExactLengthValidator<6> m_exactValidator;
	DigitsOnlyValidator m_digitsOnlyValidator;
	NotEmptyValidator m_notEmptyValidator;
public:
	bool validateInput(const std::string& text) override;
};

