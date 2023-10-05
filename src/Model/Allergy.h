#pragma once
#include <string>

struct Allergy
{
	enum Type {allergy, intolerance};
	enum Category {Food, Medication, Enviornment, Biologic };
	enum Criticality {Low, High, UnableToAsses };
	enum ClinicalStatus {
		Active,
		Recurrence, 
		Relapse,
		Inactive,
		Remission,
		Resolved 
	};

	enum VerificationStatus {
		Unconfirmed,
		Provisional,
		Differential,
		Confirmed,
		Refuted,
		EnteredInError
	};
	
	enum class NumenclatureType {
		type, category, criticality, clinicalStatus, verificationStatus
	};

	std::string lrn;
	std::string nrn;
	
	Type type = Type::allergy;
	Category category = Category::Food;
	Criticality criticality = Criticality::Low;
	ClinicalStatus clinicalStatus = ClinicalStatus::Active;
	VerificationStatus verificationStatus = VerificationStatus::Confirmed;

	int getHisNumenclatureIndex(NumenclatureType type) const;

	std::string description;
	
};