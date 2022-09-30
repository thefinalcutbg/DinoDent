#include "PatientInfoPresenter.h"
#include "Presenter/PatientDialogPresenter.h"
#include "Presenter/AllergiesDialogPresenter.h"
#include "Database/DbPatient.h"
#include "View/ModalDialogBuilder.h"

PatientInfoPresenter::PatientInfoPresenter(IPatientTileInfo* view, std::shared_ptr<Patient> p) :
    view(view), patient(p)
{
    if (User::practice().nzok_contract->nra_pass.size() &&
        User::settings().getNraStatusAuto
        )
    {
        nraClicked(false);
    }
}

void PatientInfoPresenter::setDate(const Date& date)
{
    current_date = date;
    if(view)view->setPatient(*patient, patient->getAge(current_date));
}

void PatientInfoPresenter::nraClicked(bool showDialog)
{
    m_showInsuranceDialog = showDialog;

    if (patient->insuranceStatus) {
        setInsuranceStatus(patient->insuranceStatus.value());
        return;
    }

    nraStatusServ.sendRequest(*patient.get(),

        [=](auto status) { this->setInsuranceStatus(status);}

    );
}

void PatientInfoPresenter::diagnosisClicked()
{
    nhifDiagnosisServ.sendRequest(
        patient->type,
        patient->id,


        [=](const std::string& currentDiseases) {

            if (currentDiseases.empty()) {
                ModalDialogBuilder::showMessage(u8"Няма данни в рецептурната книжка");
                return;
            }

            Patient patient = *this->patient;
            patient.currentDiseases = currentDiseases;

            AllergiesDialogPresenter p(patient);
            auto result = p.openDialog();

            if (!result) return;

            auto success = DbPatient::updateAllergies(patient.rowid, patient.allergies, patient.currentDiseases, patient.pastDiseases);

            if (!success) return;

            *this->patient = patient;

            view->setPatient(patient, patient.getAge(current_date));
        }
    );
}

void PatientInfoPresenter::patientTileClicked()
{
    if (patient == nullptr) return;

    PatientDialogPresenter p{ *patient };

    auto patient = p.open();

    if (!patient.has_value()) return;

    *this->patient = patient.value();

   // tabPresenter->refreshPatientTabNames(patient->rowid);

    view->setPatient(*patient, patient->getAge(current_date));
}

void PatientInfoPresenter::allergiesTileClicked()
{
    AllergiesDialogPresenter p(*patient.get());

    auto data = p.openDialog();

    if (!data.has_value()) return;

    auto& d = data.value();

    auto success = DbPatient::updateAllergies(patient->rowid, d.allergies, d.current, d.past);

    if (!success) return;

    patient->allergies = d.allergies;
    patient->currentDiseases = d.current;
    patient->pastDiseases = d.past;

    view->setPatient(*patient, patient->getAge(current_date));
}

void PatientInfoPresenter::setCurrent()
{
    view->setPresenter(this);
    view->setPatient(*patient, patient->getAge(current_date));
}

void PatientInfoPresenter::setInsuranceStatus(const std::optional<InsuranceStatus>& status_result)
{
    if (!status_result) {
        return;
    }

    patient->insuranceStatus = status_result.value();

    view->setPatient(*patient, patient->getAge(current_date));

    if (m_showInsuranceDialog) {
        m_showInsuranceDialog = false;
        ModalDialogBuilder::showMessage(status_result->getYearsText());
    }
}