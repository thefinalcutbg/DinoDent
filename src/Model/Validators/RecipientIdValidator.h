#pragma once
#include "BulstatValidator.h"
#include "PatientValidators.h"

class RecipientIdValidator : public Validator
{
	BulstatValidator bulstat_validator;
	EgnValidator egn_validator;
	Ln4Validator ln4_validator;

	inline static const std::string s_err = "Невалиден идентификатор";

public:
	RecipientIdValidator() { _errorMsg = &s_err; };
	bool validateInput(const std::string& text) override;

};