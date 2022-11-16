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

public:
	bool validateInput(const std::string& text) override;
};

template<int minimum>
class MinimumLengthValidator : public Validator
{

	inline static const std::string mustBeAtLeast = 
		"Това поле задължително трябва да има поне " + std::to_string(minimum) + " символа";
public:
	bool validateInput(const std::string& text) override
	{
		_errorMsg = &mustBeAtLeast;

		return (text.length() >= minimum);
	};
};

template<int exact>
class ExactLengthValidator : public Validator
{
	inline static const std::string mustBeExactly =
		"Това поле трябва да съдържа точно " + std::to_string(exact) + " символа";

public:
	bool validateInput(const std::string& text) override
	{
		_errorMsg = &mustBeExactly;

		return (text.length() == exact);
	};
};