#include "Model/Prescription/Prescription.h"

namespace DbPrescription
{
	long long insert(const Prescription& prescription);
	Prescription get(long long rowid);
	bool update(const Prescription& prescription);

}