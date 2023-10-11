#include "MedicalStatusPresenter.h"

#include <algorithm>

#include "Database/DbPatient.h"
#include "View/ModalDialogBuilder.h"
#include "Model/FreeFunctions.h"

MedicalStatusPresenter::MedicalStatusPresenter(Patient& p) : patient(p)
{
	p.allergies = DbPatient::getAllergies(p.rowid);
	p.medStats = DbPatient::getMedicalStatuses(p.rowid);
}

bool MedicalStatusPresenter::awaitingReply()
{
	return
		fetch_service.awaitingReply() ||
		report_service.awaitingReply() ||
		edit_service.awaitingReply();
}

void MedicalStatusPresenter::loadAllergiesFromHis()
{
	if (awaitingReply()) return;

	fetch_service.sendRequest(patient, User::practice().rziCode,

		[&](const std::vector<Allergy> allergies) {

			std::vector<Allergy>::iterator it;

			it = std::remove_if(
				patient.allergies.begin(),
				patient.allergies.end(), 
				[](const Allergy& a) { 
					return !a.nrn.empty(); 
				}
			);

			patient.allergies.erase(it, patient.allergies.end());
			
			for (auto& a : allergies) {

				if (a.verificationStatus == Allergy::EnteredInError) continue;
				
				patient.allergies.insert(patient.allergies.begin(), a);
			}
			
			DbPatient::updateAllergies(patient.rowid, patient.allergies);
			view->setAllergies(patient.allergies);
		}
	);

}

void MedicalStatusPresenter::loadICDFromNHIS()
{
	
}

void MedicalStatusPresenter::addAllergy()
{

	if (awaitingReply()) return;

	auto result = ModalDialogBuilder::openAllergyDialog();

	if (!result) return;

	result->lrn = FreeFn::getUuid();

	patient.allergies.push_back(result.value());

	DbPatient::updateAllergies(patient.rowid, patient.allergies);

	view->setAllergies(patient.allergies);
}

void MedicalStatusPresenter::removeAllergy(int idx)
{
	if (awaitingReply()) return;

	if (idx < 0 || idx >= patient.allergies.size()) return;

	if (patient.allergies[idx].nrn.empty()) {

		patient.allergies.erase(patient.allergies.begin() + idx);

		DbPatient::updateAllergies(patient.rowid, patient.allergies);

		view->setAllergies(patient.allergies);

		return;
	}

	edit_service.sendRequest(patient.allergies[idx],
		[=](bool success)
		{
			if (!success) return;

			loadAllergiesFromHis();
		},
		true
	);
	
}

void MedicalStatusPresenter::editAllergy(int idx)
{
	if (awaitingReply()) return;

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


	if (idx < 0 || idx >= patient.allergies.size()) return;

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

				loadAllergiesFromHis();
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

	patient.medStats = { 
		.condition = view->getCurrentDiseases(), 
		.history = view->getPastDiseases() 
	};

	DbPatient::updateMedStatus(patient.rowid, patient.medStats);
}
