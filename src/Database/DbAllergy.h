#pragma once
#include <vector>
#include "Model/Allergy.h"
#include "Model/Patient.h"

namespace DbAllergy {

	bool updateAllergies(long long patientRowid, const std::vector<Allergy> allergies);
	std::vector<Allergy> getAllergies(long long patientRowid);

}