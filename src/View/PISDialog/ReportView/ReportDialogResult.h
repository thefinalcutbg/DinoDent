#pragma once

#include <string>
#include <optional>
#include "Model/Date.h"

struct ReportDialogResult
{
	int month;
	int year;
	std::string path;
};