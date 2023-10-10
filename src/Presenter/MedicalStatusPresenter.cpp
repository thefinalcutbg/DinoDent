#include "MedicalStatusPresenter.h"
#include "Database/DbPatient.h"
#include "View/ModalDialogBuilder.h"
#include "Model/FreeFunctions.h"
#include <algorithm>

MedicalStatusPresenter::MedicalStatusPresenter(Patient& p) : patient(p)
{
	p.allergies = DbPatient::getAllergies(p.rowid);
	p.medStats = DbPatient::getMedicalStatuses(p.rowid);
}

void MedicalStatusPresenter::okPressed()
{
	view->close();

	patient.medStats = { view->getCurrentDiseases(), view->getPastDiseases() };

	DbPatient::updateMedStatus(patient.rowid, patient.medStats);
}

void MedicalStatusPresenter::loadAllergiesFromHis()
{
	fetch_service.sendRequest(patient, User::practice().rziCode,
		[&](const std::vector<Allergy> allergies) {

			for (auto& new_a : allergies) {

				bool allergyExists = false;

				for (auto& old_a : patient.allergies)
				{
					if (old_a.nrn == new_a.nrn)
					{
						allergyExists = true;
						old_a = new_a;
					}
				}

				if(!allergyExists) patient.allergies.push_back(new_a);

			}

			DbPatient::updateAllergies(patient.rowid, patient.allergies);
			view->setAllergies(patient.allergies);
		}
	);

}

void MedicalStatusPresenter::addAllergy()
{
	auto result = ModalDialogBuilder::openAllergyDialog();

	if (!result) return;

	result->lrn = FreeFn::getUuid();

	patient.allergies.push_back(result.value());

	DbPatient::updateAllergies(patient.rowid, patient.allergies);

	view->setAllergies(patient.allergies);
}

void MedicalStatusPresenter::removeAllergy(int idx)
{
	if (idx < 0 || idx >= patient.allergies.size()) return;
}

void MedicalStatusPresenter::editAllergy(int idx)
{
	if (idx < 0 || idx >= patient.allergies.size()) return;

	auto result = ModalDialogBuilder::openAllergyDialog(patient.allergies[idx]);

	if (!result) return;

	result->nrn = patient.allergies[idx].nrn;
	result->lrn = patient.allergies[idx].lrn;

	patient.allergies[idx] = result.value();

	if (patient.allergies[idx].nrn.size()) patient.allergies[idx].edited = true;

	DbPatient::updateAllergies(patient.rowid, patient.allergies);

	view->setAllergies(patient.allergies);
}

void MedicalStatusPresenter::sendToHis(int idx)
{

	if (!patient.allergies[idx].nrn.size())
	{
		report_service.sendRequest(patient, User::practice().rziCode, patient.allergies[idx],
			[=](const std::string& nrn)
			{
				patient.allergies[idx].nrn = nrn;
				DbPatient::updateAllergies(patient.rowid, patient.allergies);
				view->setAllergies(patient.allergies);
			}
		);
	}
	else
	{
		edit_service.sendRequest(patient.allergies[idx],
			[=](bool success)
			{
				if (!success) return;

				patient.allergies[idx].edited = false;
				DbPatient::updateAllergies(patient.rowid, patient.allergies);
				view->setAllergies(patient.allergies);
			}
		);

	}
	
}

void MedicalStatusPresenter::openDialog()
{
	MedicalStatusDialog d(this);
	view = &d;

	view->setAllergies(patient.allergies);
	view->setCurrentDiseases(patient.medStats.condition);
	view->setPastDiseases(patient.medStats.history);

	d.exec();
}
