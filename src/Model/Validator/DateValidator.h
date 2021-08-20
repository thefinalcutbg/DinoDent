#pragma once
#include "AbstractValidator.h"
#include "Model/Date.h"

class DateValidator : public Validator
{

	Date dateMin;
	Date dateMax;

	std::string _minErrorMsg;
	std::string _maxErrorMsg;

public:
	DateValidator();
	Date getMin();
	Date getMax();
	void setMaxDate(const Date& maxDate);
	void setMinDate(const Date& minDate);
	bool validateInput(const Date& date) override;

	void setMaxErrorMsg(const std::string& errorMsg);
	void setMinErrorMsg(const std::string& errorMsg);
	
};

