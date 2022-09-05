#pragma once

#include <optional>

#include "View/Interfaces/IMedicationDialog.h"
#include "Model/Perscription/Medication.h"

class MedicationPresenter
{
	IMedicationDialog* view{ nullptr };

	Medication m_medication;

public:
	
	MedicationPresenter();
	void quantityChanged(int quantity);
	void medicationChanged(const std::string& medication);
	void substitutionChanged(bool allowSubstitution);
	void addDosage();
	std::optional<Medication> openDialog();
	void setView(IMedicationDialog* view);

};

