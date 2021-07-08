#pragma once
#include "AbstractValidator.h"
#include "DateValidator.h"

class ProcedureDateValidator : public Validator
{
	Date _ambDate;
	Date _dayBefore18;

	DateValidator _validator; //composition over inheritance, right?

public:
	ProcedureDateValidator(Date ambDate, Date patientBirth);
	void setProcedure(int code, bool nzok);
	bool validateInput(const Date& date) override;
	Date getValidDate();
};

