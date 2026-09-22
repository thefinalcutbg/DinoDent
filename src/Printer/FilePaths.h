#pragma once
#include <string>
#include <vector>
#include "Model/GlobalSettingsData.h"

struct AmbList;
struct Prescription;
struct Patient;
struct Invoice;
struct TreatmentPlan;

namespace FilePaths {

	void setSettings(const std::string& dir, const std::vector<DirType> subdirStructure);

	enum DeclarationType : long long { Denture, HIRBNo, Consent, GDPR, Custom };

	std::string get(const AmbList& amb, const Patient& patient, bool nhifForm);
	std::string get(const Prescription& prescr, const Patient& patient);
	std::string get(const Invoice& invoice);
	std::string get(DeclarationType declType, const Patient& patient);
    std::string get(const TreatmentPlan& plan, const Patient& patient);
}
