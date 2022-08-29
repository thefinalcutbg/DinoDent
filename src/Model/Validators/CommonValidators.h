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
		u8"Това поле задължително трябва да има поне " + std::to_string(minimum) + u8" символа";
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
		u8"Това поле трябва да съдържа точно " + std::to_string(exact) + u8" символа";

public:
	bool validateInput(const std::string& text) override
	{
		_errorMsg = &mustBeExactly;

		return (text.length() == exact);
	};
};