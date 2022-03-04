#pragma once
#include "AbstractValidator.h"
#include "CommonValidators.h"

class RziValidator : public Validator
{
	NotEmptyValidator m_notEmpty;
	DigitsOnlyValidator m_digitsOnly;
	ExactLengthValidator<10> m_exactLenght;


public:
	bool validateInput(const std::string& text) override;
};