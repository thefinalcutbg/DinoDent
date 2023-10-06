#include "Allergy.h"

int Allergy::getHisNumenclatureIndex(NumenclatureType numType) const
{
	switch (numType)
	{
		case NumenclatureType::type: return type + 1;
		case NumenclatureType::category: return category + 1;
		case NumenclatureType::criticality: return criticality + 1;
		case NumenclatureType::clinicalStatus:
			switch (clinicalStatus)
			{
				case ClinicalStatus::Active: return 10;
				case ClinicalStatus::Recurrence: return 11;
				case ClinicalStatus::Relapse: return 12;
				case ClinicalStatus::Inactive: return 20;
				case ClinicalStatus::Remission: return 21;
				case ClinicalStatus::Resolved: return 22;
				default: return 0;
			}
		case NumenclatureType::verificationStatus:
			switch (verificationStatus)
			{
				case VerificationStatus::Unconfirmed: return 10;
				case VerificationStatus::Provisional: return 11;
				case VerificationStatus::Differential: return 12;
				case VerificationStatus::Confirmed: return 20;
				case VerificationStatus::Refuted: return 30;
				case VerificationStatus::EnteredInError: return 40;
				default: return 0;
			}
		default: return 0;
	}
}
