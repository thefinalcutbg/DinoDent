#pragma once
#include "AbstractValidator.h"
#include "Model/Date.h"

class DateValidator : public Validator
{

	Date min;
	Date max;

	static std::string error;

public:
	DateValidator();
	Date getMin();
	Date getMax();
	void setMaxDate(const Date& maxDate);
	void setMinDate(const Date& minDate);
	void setAmbListDate(const Date& amb_date);
	bool validate(int d, int m, int y) override;

};

