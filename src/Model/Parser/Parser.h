#pragma once

#include <string>
#include <vector>
#include "Model/Procedure/ProcedureTemplate.h"
#include "Model/XML/AmbListXML.h"
#include "Model/XML/InvoiceXML.h"
#include <optional>
#include "Model/User/User.h"

struct PerioStatus;
class ToothContainer;
struct Procedure;
struct DetailsSummary;
struct NzokContract;

namespace Parser
{
	std::string write(const PerioStatus& status);
	std::string write(const ToothContainer& status);
	std::string write(const Procedure& procedure);
	std::string write(const std::optional<NzokContract>& contract);
	std::string write(const std::vector<ProcedureTemplate>& priceList);

	void parse(const std::string& jsonString, Procedure& procedure);
	void parse(const std::string& jsonString, DetailsSummary& summary);
	void parse(const std::string& jsonString, PerioStatus& status);
	void parse(const std::string& jsonString, ToothContainer& status);
	

	std::vector<ToothXML> getTeethXML(const std::string& jsonString);
	std::vector<ProcedureTemplate> getPriceList(const std::string& priceList);
	std::string parseDiagnosis(const std::string& jsonProcedureString);
	std::optional<NzokContract> parseContract(const std::string& jsonString);
};