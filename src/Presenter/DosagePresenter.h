#pragma once
#include <optional>

#include "Model/Prescription/Dosage.h"
#include "View/Interfaces/IDosageDialog.h"
#include "View/ModalDialogBuilder.h"
class DosagePresenter
{

	IDosageDialog* view{ nullptr };

	Dosage m_dosage;
	bool ok_pressed{ false };

public:

	DosagePresenter(int medicationFormUnit);
	DosagePresenter(const Dosage& dosage) : m_dosage(dosage) {};
	
	void setView(IDosageDialog* view);

	void additionalInstructionsChanged(const std::string& text) {
		m_dosage.additionalInstructions = text;
		view->setParsed(m_dosage.parse());
	}

	void setAsNeeded(bool asNeeded) {
		m_dosage.asNeeded = asNeeded;
	}

	void periodUnitChanged(int idx) {
		if (m_dosage.period.setUnit(idx))
			view->setParsed(m_dosage.parse());
	}

	void boundsUnitChanged(int idx) {
		if (m_dosage.bounds.setUnit(idx))
			view->setParsed(m_dosage.parse());
	};

	void routeChanged(const std::string& route) {

		if(m_dosage.route.set(route))
			view->setParsed(m_dosage.parse());
	};

	void periodValueChanged(double value) {
		m_dosage.period.value = value;
		view->setPeriodList(m_dosage.period.getUnitNamesList());
		view->setParsed(m_dosage.parse());
	}

	void boundsValueChanged(double value) {
		m_dosage.bounds.value = value;
		view->setBoundsList(m_dosage.bounds.getUnitNamesList());
		view->setParsed(m_dosage.parse());
	};

	void frequencyChanged(int value) {
		m_dosage.frequency = value;
		view->setParsed(m_dosage.parse());
	};

	void formNameChanged(const std::string& form)
	{
		m_dosage.doseQuantity.setUnit(form); 

		view->setParsed(m_dosage.parse());

	}

	void doseQuantityValueChanged(double value) {
		m_dosage.doseQuantity.value = value;

		view->setDoseFormCompletionList(m_dosage.doseQuantity.unitNames());
		view->setDosageUnit(m_dosage.doseQuantity.getUnitName());
		view->setParsed(m_dosage.parse());
	}

	void whenTagAdded(const std::string& whenText) {

		if(!m_dosage.when.addTag(whenText)) return;

		view->setWhenTags(m_dosage.when.getTags(), m_dosage.when.offsetAllowed());
		view->setParsed(m_dosage.parse());
	}

	void removeTag(int tagIdx)
	{
		if (!m_dosage.when.removeTag(tagIdx)) return;

		view->setWhenTags(m_dosage.when.getTags(), m_dosage.when.offsetAllowed());
		view->setParsed(m_dosage.parse());
	}

	void offsetChanged(int offsetValue) {
		m_dosage.when.setOffset(offsetValue);
		view->setParsed(m_dosage.parse());
	}

	void enableOffset(bool enabled){
		m_dosage.when.enableOffset(enabled);
		view->setParsed(m_dosage.parse());
	}

	void okPressed();

	std::optional<Dosage> openDialog();

};

