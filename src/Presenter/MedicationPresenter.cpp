#include "MedicationPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "DosagePresenter.h"
#include <QDebug>
MedicationPresenter::MedicationPresenter()
{
}

void MedicationPresenter::quantityChanged(int quantity)
{
	m_medication.quantityValue = quantity;

	qDebug() << "quantityChanged" << quantity;

	if (quantity == 1) {
		view->setQuantityListNames(u8"Опаковка", u8"Лекарствена форма");
	}
	else
	{
		view->setQuantityListNames(u8"Опаковки", u8"Лекарствени форми");
	}
}

void MedicationPresenter::medicationChanged(const std::string& mediName)
{
	m_medication.setName(mediName);
	view->setFormLabel(m_medication.formStr());
}

void MedicationPresenter::substitutionChanged(bool allowSubstitution)
{
	m_medication.isSubstitutionAllowed = allowSubstitution;
}

void MedicationPresenter::addDosage()
{
	DosagePresenter p(m_medication.form);
	p.openDialog();
}

std::optional<Medication> MedicationPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);

	return std::optional<Medication>();
}

void MedicationPresenter::setView(IMedicationDialog* view)
{
	this->view = view;
}
