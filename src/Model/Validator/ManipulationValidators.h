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
	void setAmbListDate(const Date& amb_date);
	bool validate(const std::string& text) override;
	bool validate(const Date& date);
};

class BridgeRangeValidator : public Validator
{

public:
	bool validate(int begin, int end) override;

};

class SurfaceValidator : public Validator
{
public:
	bool validate(const std::array<bool, 6>& surfaces) override;
};

class EmptyFieldValidator : public Validator
{
public: 
	bool validate(const std::string& text) override;
};