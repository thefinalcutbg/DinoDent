#pragma once
#include "CommonValidators.h"

class LpkValidator : public Validator
{
	NotEmptyValidator m_notEmptyValidator;
	DigitsOnlyValidator m_digitsValidator;
	MinimumLengthValidator<9> m_minimumLengthValidator;
public:
	bool validateInput(const std::string& text) override;
};
