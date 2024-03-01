#pragma once
#include "CommonValidators.h"

class BulstatValidator : public Validator
{
	NotEmptyValidator m_notEmptyValidator;
	DigitsOnlyValidator m_digitsValidator;
	MinimumLengthValidator<9> m_minimumLengthValidator;
public:
	bool validateInput(const std::string& text) override;
};

class IbanValidator : public Validator
{
public:
	bool validateInput(const std::string& text) override;
};

class BICValidator : public Validator
{
public:
	bool validateInput(const std::string& text) override;
};