#pragma once
#include "AbstractValidator.h"
#include "CommonValidators.h"
#include "DateValidator.h"

class EgnValidator : public Validator
{
	static const std::string invalid;

	DigitsOnlyValidator digits_only;
	DateValidator date_validator;
	NotEmptyValidator not_empty;
	
public:
	EgnValidator();
	bool validateInput(const std::string& text);
};


class Ln4Validator : public Validator
{
	DigitsOnlyValidator digits_only;
	NotEmptyValidator not_empty;
	static const std::string invalid;
public:
	Ln4Validator();
	bool validateInput(const std::string& text);
};

class HIRBNoValidator : public Validator
{
	DigitsOnlyValidator digits_only;
public:
	static const std::string invalid;
	HIRBNoValidator();
	bool validateInput(const std::string& text);
};



class CityValidator : public Validator
{
	static const std::string invalidCity;

public:

	bool validateInput(const std::string& text);
};

