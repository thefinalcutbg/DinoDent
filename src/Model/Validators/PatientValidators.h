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

class SSNValidator : public Validator
{
	NotEmptyValidator notEmpty_validator;
	DigitsOnlyValidator digits_only_validator;
	static const std::string invalid;
public:
	SSNValidator();
	bool validateInput(const std::string& text);
};

class CityValidator : public Validator
{
	static const std::string invalidCity;

public:
	CityValidator() { _errorMsg = &invalidCity; }
	bool validateInput(const std::string& text);
};

class EgnLnchValidator : public Validator
{

	EgnValidator egn_val;
	Ln4Validator lnch_val;

	static inline const std::string invalid = "Невалиден ЕГН/ЛНЧ";
public:
	EgnLnchValidator();
	bool validateInput(const std::string& text);

};
