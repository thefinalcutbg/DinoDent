#pragma once
#include "AbstractValidator.h"

class NotEmptyValidator : public Validator
{
	static const std::string couldntBeEmpty;

public:
	NotEmptyValidator();
	bool validateInput(const std::string& text) override;
};

class CyrillicValidator : public Validator
{
	static const std::string mustBeCyrillic;

public:

	bool validateInput(const std::string& text) override;
};

class DigitsOnlyValidator : public Validator
{
	static const std::string mustBeNumber;

public:
	bool validateInput(const std::string& text) override;
};
