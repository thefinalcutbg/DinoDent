#pragma once
#include <string>
#include <Model/Date.h>

struct AmbListRow
{
	std::string id;
	int ambNumber;
	Date date;
	std::string patientName;
	std::string patientId;
};
