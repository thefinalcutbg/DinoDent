#include "MedicalStatusPresenter.h"
#include "Database/DbPatient.h"
#include "View/ModalDialogBuilder.h"
#include "Model/FreeFunctions.h"
#include "Database/DbAllergy.h"

MedicalStatusPresenter::MedicalStatusPresenter(Patient& p) : patient(p), allergies(DbAllergy::getAllergies(p.rowid))
{
	//ensures consistency
	p.allergies = allergies;
}

void MedicalStatusPresenter::okPressed()
{
	view->close();

	//save to db

	other_statuses = { view->getCurrentDiseases(), view->getPastDiseases() };

	DbPatient::updateMedStatus(patient.rowid, other_statuses);

	patient.medStats = other_statuses;
	

}

void MedicalStatusPresenter::loadAllergiesFromHis()
{


}

void MedicalStatusPresenter::addAllergy()
{
	auto result = ModalDialogBuilder::openAllergyDialog();

	if (!result) return;

	result->lrn = FreeFn::getUuid();

	allergies.push_back(result.value());

	DbAllergy::updateAllergies(patient.rowid, allergies);

	patient.allergies = allergies;

	view->setAllergies(allergies);
}

void MedicalStatusPresenter::removeAllergy(int idx)
{
	
}

void MedicalStatusPresenter::editAllergy(int idx)
{
	if (idx < 0 || idx >= allergies.size()) return;

	auto result = ModalDialogBuilder::openAllergyDialog(allergies[idx]);

	if (!result) return;

	DbAllergy::updateAllergies(patient.rowid, allergies);

	patient.allergies = allergies;

	allergies.push_back(result.value());

	view->setAllergies(allergies);
}

void MedicalStatusPresenter::sendToHis(int idx)
{
}

void MedicalStatusPresenter::openDialog()
{
	MedicalStatusDialog d(this);
	view = &d;

	d.exec();
}
