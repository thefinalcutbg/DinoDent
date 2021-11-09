#pragma once

#include <string>

#include "Model/Tooth/ToothContainer.h"
#include "Model/PerioStatus.h"
#include "Model/Procedure/Procedure.h"
#include "Model/Procedure/TableStructs.h"

namespace Parser
{
	std::string write(const PerioStatus& status);
	std::string write(const ToothContainer& status);
	std::string write(const Procedure& procedure);

	void parse(const std::string& jsonString, Procedure& procedure);
	void parse(const std::string& jsonString, DetailsSummary& summary);
	void parse(const std::string& jsonString, PerioStatus& status);
	void parse(const std::string& jsonString, ToothContainer& status);
};