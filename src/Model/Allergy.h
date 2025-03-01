#pragma once
#include <string>
#include <optional>
#include "Model/Date.h"

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
	std::string description;
	
	Type type = Type::allergy;
	Category category = Category::Medication;
	Criticality criticality = Criticality::Low;
	ClinicalStatus clinicalStatus = ClinicalStatus::Active;
	VerificationStatus verificationStatus = VerificationStatus::Confirmed;

	std::optional<Date> lastOccurrence;

	bool edited{ false };

	int getHisNumenclatureIndex(NumenclatureType num_type) const;
	void setHisNumenclature(NumenclatureType num_type, int index);
	std::string getNumText(NumenclatureType num_type) const;
	std::string getDescription() const;
};