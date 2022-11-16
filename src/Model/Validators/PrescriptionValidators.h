#pragma once
#include "AbstractValidator.h"

class MedicationNameValidator : public Validator
{
	static inline const std::string invalidMedication = "Невалидно лекарство";

public:
	MedicationNameValidator() { _errorMsg = &invalidMedication; }
	bool validateInput(const std::string& text);

};


class RouteFormValidator : public Validator
{
	static inline const std::string invalidForm = "Невалиднен път на въвеждане";

public:
	RouteFormValidator() { _errorMsg = &invalidForm; }
	bool validateInput(const std::string& text);

};

class WhenTextValidator : public Validator
{
	static inline const std::string invalidForm = "Невалидно указание за прием";

public:
	WhenTextValidator() { _errorMsg = &invalidForm; }
	bool validateInput(const std::string& text);

};