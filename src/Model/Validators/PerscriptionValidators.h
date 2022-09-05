#pragma once
#include "AbstractValidator.h"

class MedicationNameValidator : public Validator
{
	static inline const std::string invalidMedication = u8"Невалидно лекарство";

public:
	MedicationNameValidator() { _errorMsg = &invalidMedication; }
	bool validateInput(const std::string& text);

};

class DosageFormValidator : public Validator
{
	static inline const std::string invalidForm = u8"Невалидна форма на приложение";

public:
	DosageFormValidator() { _errorMsg = &invalidForm; }
	bool validateInput(const std::string& text);

};

class RouteFormValidator : public Validator
{
	static inline const std::string invalidForm = u8"Невалиднен път на въвеждане";

public:
	RouteFormValidator() { _errorMsg = &invalidForm; }
	bool validateInput(const std::string& text);

};