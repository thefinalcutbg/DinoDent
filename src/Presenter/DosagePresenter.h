#pragma once
#include <optional>

#include "Model/Perscription/Dosage.h"
#include "View/Interfaces/IDosageDialog.h"

class DosagePresenter
{

	IDosageDialog* view{ nullptr };

	Dosage m_dosage;

public:

	DosagePresenter(int medicationFormUnit);
	
	void setView(IDosageDialog* view);

	void routeChanged(const std::string& route) {
		m_dosage.route.set(route);
	};

	void durationValueChanged(int value) {
		m_dosage.duration.value = value;
		view->setDurationList(m_dosage.duration.getUnitNamesList());
	}

	void periodValueChanged(int value) {
		m_dosage.period.value = value;
		view->setPeriodList(m_dosage.period.getUnitNamesList());
	};

	void frequencyChanged(int value) {
		m_dosage.frequency = value;
	};

	void formNameChanged(const std::string& form)
	{
		if (m_dosage.doseQuantity.setUnit(form) == true) {
			view->setDosageLabel(m_dosage.doseQuantity.getParsedUnit());
		}
	}

	void doseQuantityValueChanged(double value) {
		m_dosage.doseQuantity.value = value;
		view->setDosageLabel(m_dosage.doseQuantity.getParsedUnit());
		view->setDoseFormCompletionList(m_dosage.doseQuantity.unitNames());
		view->setDosageUnit(m_dosage.doseQuantity.getUnitName());
	}

	std::optional<Dosage> openDialog();

};

