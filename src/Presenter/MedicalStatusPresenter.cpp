#include "MedicalStatusPresenter.h"

#include <algorithm>

#include "Database/DbPatient.h"
#include "Model/User.h"
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
	nhis_diag_service.sendRequest(
		patient,
		[&](
			const std::vector<ICD10>& current,
			const std::vector<ICD10>& past
			
		) {

			if (current.empty() && past.empty()) {
				ModalDialogBuilder::showMessage("Няма данни в рецептурната книжка");
				return;
			}

			//current

			auto list = view->getCurrentDiseases();

			std::vector<MedicalStatus> toInsert;

			for (auto& icd : current) {
				toInsert.push_back(MedicalStatus{ .diagnosis = icd });
			}

            MedicalStatuses::insertUnique(toInsert, list);

			view->setCurrentDiseases(list);

			//past (code duplication)

			list = view->getPastDiseases();

			toInsert.clear();

			for (auto& icd : past) {
				toInsert.push_back(MedicalStatus{ .diagnosis = icd });
			}

            MedicalStatuses::insertUnique(toInsert, list);
	
			view->setPastDiseases(list);
		}
        );
}

void MedicalStatusPresenter::loadICDFromHIS()
{
    fetch_condition_service.sendRequest(
        patient,
        User::practice().rziCode,
        [&](
            const std::vector<MedicalStatus>& current,
            const std::vector<MedicalStatus>& past

            ) {

            if (current.empty() && past.empty()) {
                ModalDialogBuilder::showMessage("Не са открити резултати в НЗИС");
                return;
            }

            auto list = view->getCurrentDiseases();

            MedicalStatuses::insertUnique(current, list);

            view->setCurrentDiseases(list);

            list = view->getPastDiseases();

            MedicalStatuses::insertUnique(past, list);

            view->setPastDiseases(list);
        }
        );
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

	if (!patient.allergies[idx].nrn.empty()) {

		bool answer = ModalDialogBuilder::askDialog(
			"Алергията ще бъде изтрита само от локалната база данни. "
			"Ако искате да я изтриете от НЗИС, редактирайте верификационния ѝ статус на \"Погрешно въведен\" и я изпратете отново\n"
			"Желаете ли да продължите?"
		);

		if (!answer) return;
	}

	patient.allergies.erase(patient.allergies.begin() + idx);

	DbPatient::updateAllergies(patient.rowid, patient.allergies);

	view->setAllergies(patient.allergies);

	return;
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

void MedicalStatusPresenter::sendAllergyToHis(int idx)
{


	if (idx < 0 || idx >= patient.allergies.size()) return;

	if (!patient.allergies[idx].nrn.size())
	{
        report_service.sendRequest(patient, patient.allergies[idx],
            [=, this](const std::string& nrn)
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
            [=, this](const std::string& nrn)
			{
				for (auto& a : patient.allergies) {
					
					if (a.nrn == nrn) {
						a.edited = false;

						DbPatient::updateAllergies(patient.rowid, patient.allergies);
						view->setAllergies(patient.allergies);

						return;
					}
				}


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
