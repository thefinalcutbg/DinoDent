#pragma once
#include <string>
#include <vector>
#include "Model/GlobalSettingsData.h"

struct AmbList;
struct Prescription;
struct Patient;
struct Invoice;

namespace FilePaths {

	void setSettings(const std::string& dir, const std::vector<DirType> subdirStructure);

	enum DeclarationType{ Denture, HIRBNo, Consent, GDPR };

	std::string get(const AmbList& amb, const Patient& patient);
	std::string get(const Prescription& prescr, const Patient& patient);
	std::string get(const Invoice& invoice);
	std::string get(DeclarationType declType, const Patient& patient);
}