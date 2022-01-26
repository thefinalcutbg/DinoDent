#pragma once
#include <string>
#include <variant>
#include <Model/Date.h>

struct AmbRow
{
	const std::string id;
	const int ambNumber;
	bool nzok;
	const Date date;
	const std::string patientId;
	const std::string patientName;
	const std::string patientPhone;
};

struct PatientRow
{
	const std::string patientId;
	const std::string patientName;
	const std::string patientPhone;
};

struct PerioRow
{
	const std::string id;
	const Date date;
	const std::string patientId;
	const std::string patientName;
	const std::string patientPhone;
};
