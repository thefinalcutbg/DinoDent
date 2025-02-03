#pragma once

#include <string>
#include <vector>
#include <optional>
#include "Model/UserStructs.h"
#include "Model/Dental/NhifSheetData.h"
#include "Model/Prescription/Dosage.h"
#include "Model/Dental/HISDental.h"

struct PerioStatus;
class ToothContainer;
struct Procedure;
struct DetailsSummary;
struct NhifContract;
struct Invoice;
struct NhifData;
struct HISProcedureResult;

namespace Parser
{
	std::string write(const PerioStatus& status);
	std::string write(const ToothContainer& status);
	std::string write(const std::optional<NhifContract>& contract);
	std::string write(const Invoice& inv);
	std::string write(const Settings& settings);
	std::string write(const std::vector<Dosage>& dosage);
	std::string write(const ToothContainer& status);
	std::string write(const HISProcedureResult& hisResult);

	void parse(const std::string& jsonString, DetailsSummary& summary);
	void parse(const std::string& jsonString, PerioStatus& status);
	void parse(const std::string& jsonString, ToothContainer& status);
	void parse(const std::string& jsonString, Invoice& invoice);


	std::string parseDiagnosis(const std::string& jsonProcedureString);
	std::optional<NhifContract> parseContract(const std::string& jsonString);
	Settings parseSettings(const std::string& settings);
	std::vector<Dosage> parseDosage(const std::string& dosage);
	HISProcedureResult parseHISResult(const std::string& hisResultStr);
};