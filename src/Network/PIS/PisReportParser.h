#pragma once
#include <utility>
#include "Model/Patient.h"
#include "Model/Dental/AmbList.h"

typedef std::vector<std::pair<Patient, AmbList>> PisReportsForImport;

namespace PisReportParser {

	PisReportsForImport parse(const std::string& xmlReport);

};