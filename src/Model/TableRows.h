#pragma once
#include <string>
#include <variant>
#include <Model/Date.h>
#include "Presenter/TabPresenter/TabInstance.h"

struct RowInstance
{
	const TabType type;
	std::string rowID;
	std::string patientId;

	RowInstance(TabType t) : type(t) {}
};

struct AmbRow : public RowInstance
{
	AmbRow() : RowInstance{ TabType::AmbList } {};

	int ambNumber{-1};
	bool nzok{0};
	Date date;

	std::string patientName;
	std::string patientPhone;
};

struct PatientRow : public RowInstance
{
	PatientRow() : RowInstance{ TabType::PatientSummary } {};

	std::string name;
	std::string phone;
};

struct PerioRow : public RowInstance
{
	PerioRow() : RowInstance{ TabType::PerioList } {};

	Date date;
	std::string patientName;
	std::string patientPhone;
};

struct FinancialRow : public RowInstance
{
	FinancialRow() : RowInstance{TabType::Financial} {};

	int number {0};
	bool nzok{ 0 };
	
	Date date;
	std::string recipientId;
	std::string recipientName;
	std::string recipientPhone;
};
