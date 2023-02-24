#pragma once

#include <string>
#include <vector>
#include <optional>
#include "Model/UserStructs.h"
#include "Model/Dental/NhifSheetData.h"
#include "Model/Prescription/Dosage.h"

struct PerioStatus;
class ToothContainer;
struct Procedure;
struct DetailsSummary;
struct NzokContract;
struct Invoice;
struct NhifData;

namespace Parser
{
	std::string write(const PerioStatus& status);
	std::string write(const ToothContainer& status);
	std::string write(const Procedure& procedure);
	std::string write(const std::optional<NzokContract>& contract);
	std::string write(const std::vector<ProcedureCode>& priceList);
	std::string write(const Invoice& inv);
	std::string write(const Settings& settings);
	std::string write(const std::vector<Dosage>& dosage);

	void parse(const std::string& jsonString, Procedure& procedure);
	void parse(const std::string& jsonString, DetailsSummary& summary);
	void parse(const std::string& jsonString, PerioStatus& status);
	void parse(const std::string& jsonString, ToothContainer& status);
	void parse(const std::string& jsonString, Invoice& invoice);
	
	std::vector<ProcedureCode> getPriceList(const std::string& priceList);
	std::string parseDiagnosis(const std::string& jsonProcedureString);
	std::optional<NzokContract> parseContract(const std::string& jsonString);
	Settings parseSettings(const std::string& settings);
	std::vector<Dosage> parseDosage(const std::string& dosage);
};