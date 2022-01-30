#pragma once

#include <string>

struct PerioStatus;
class ToothContainer;
struct Procedure;
struct DetailsSummary;

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