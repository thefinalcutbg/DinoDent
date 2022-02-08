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

template<int minimum>
class MinimumLengthValidator : public Validator
{

	inline static const std::string mustBeAtLeast = 
		"Това полезадължително трябва да има поне " + std::to_string(minimum) + " символа";
public:
	bool validateInput(const std::string& text) override
	{
		_errorMsg = &mustBeAtLeast;

		return (text.length() >= minimum);
	};
};
