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

void Allergy::setHisNumenclature(NumenclatureType num_type, int index)
{
	switch (num_type) {

	case NumenclatureType::type: type = static_cast<Type>(index - 1); break;
		case NumenclatureType::category: category = static_cast<Category>(index-1); break;
		case NumenclatureType::criticality: criticality = static_cast<Criticality>(index-1); break;
		case NumenclatureType::clinicalStatus:
			switch (index)
			{
				case 10: clinicalStatus = ClinicalStatus::Active; break;
				case 11: clinicalStatus = ClinicalStatus::Recurrence; break;
				case 12: clinicalStatus = ClinicalStatus::Relapse; break;
				case 20: clinicalStatus = ClinicalStatus::Inactive; break;
				case 21: clinicalStatus = ClinicalStatus::Remission; break;
				case 22: clinicalStatus = ClinicalStatus::Resolved; break;
			}
			break;
		case NumenclatureType::verificationStatus:
			switch (index)
			{
			case 10: verificationStatus = VerificationStatus::Unconfirmed; break;
			case 11: verificationStatus = VerificationStatus::Provisional; break;
			case 12: verificationStatus = VerificationStatus::Differential; break;
			case 20: verificationStatus = VerificationStatus::Confirmed; break;
			case 30: verificationStatus = VerificationStatus::Refuted; break;
			case 40: verificationStatus = VerificationStatus::EnteredInError; break;
			}
			break;
	}
}
