#include "Model/Prescription/Prescription.h"

namespace DbPrescription
{
	long long insert(const Prescription& prescription);
	Prescription get(long long rowid);
	bool update(const Prescription& prescription);
	bool nrnExists(const std::string& nrn);
	std::vector<std::pair<long long, Medication>> getTemplates();
	bool insertTemplate(const Medication& m);
	bool deleteTemplate(long long rowid);

}