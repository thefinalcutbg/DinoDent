#pragma once

#include <optional>

#include "View/Interfaces/IMedicationDialog.h"
#include "Model/Prescription/Medication.h"

class MedicationPresenter
{
	IMedicationDialog* view{ nullptr };

	Medication m_medication;

	bool m_readOnly{ false };

	bool ok_pressed{ false };

public:
	
	MedicationPresenter();
	MedicationPresenter(const Medication& m, bool readOnly = false) : m_medication{ m }, m_readOnly(readOnly) {};
	void commonDataChanged(int quantity, bool quantityByForm, bool allowSubstitution);
	void medicationChanged(const std::string& medication);
	void priorityChanged(int idx) { m_medication.priority = static_cast<Medication::Priority>(idx); }
	void dosePeriodChanged(const std::optional<DosePeriod>& p);
	void noteChanged(const std::string& note);


	void addDosage();
	void editDosage(int idx);
	void deleteDosage(int idx);

	void okPressed();

	std::optional<Medication> openDialog();
	void setView(IMedicationDialog* view);

};

