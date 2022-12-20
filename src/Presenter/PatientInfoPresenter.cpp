#include "PatientInfoPresenter.h"
#include "Presenter/PatientDialogPresenter.h"
#include "Presenter/AllergiesDialogPresenter.h"
#include "Database/DbPatient.h"
#include "View/ModalDialogBuilder.h"
#include "View/Printer.h"

PatientInfoPresenter::PatientInfoPresenter(IPatientTileInfo* view, std::shared_ptr<Patient> p) :
    view(view), patient(p)
{}

void PatientInfoPresenter::setDate(const Date& date)
{
    current_date = date;

    view->setPresenter(this);
    view->setPatient(*patient, patient->getAge(current_date));
}

void PatientInfoPresenter::nraClicked(bool showDialog)
{
    m_showInsuranceDialog = showDialog;

    if (patient->insuranceStatus) {
        setInsuranceStatus(patient->insuranceStatus.value());
        return;
    }

    nraStatusServ.sendRequest(
        *patient.get(),
        [=](auto status) { this->setInsuranceStatus(status);},
        showDialog
    );
}

void PatientInfoPresenter::diagnosisClicked()
{
    nhifDiagnosisServ.sendRequest(
        patient->type,
        patient->id,


        [=](const std::string& currentDiseases) {

            if (currentDiseases.empty()) {
                ModalDialogBuilder::showMessage("Няма данни в рецептурната книжка");
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

void PatientInfoPresenter::printDeclarations()
{
    static std::vector<std::string> printOptions{
       "Декларация за тотални протези",
       "Декларация за валидна здравна книжка",
       "Информирано съгласие",
       "Декларация за GDPR"
    };

    static std::vector<std::string> declaratorType{
       "За осигурено лице",
       "За родител/настойник",
       "Празна бланка" 
    };

    int result = ModalDialogBuilder::openButtonDialog(printOptions, "Изберете декларация");

    switch (result)
    {
        case 0:
        {

            int decl_result = 
                ModalDialogBuilder::openButtonDialog(
                    declaratorType,
                    printOptions[0]
                );
          
            if(decl_result == -1) return;

            Print::DeclaratorType type = static_cast<Print::DeclaratorType>(decl_result);
              
            Print::printDentureDeclaration(*patient, type);

            return;
        }

        case 1:
        {
            {

                int decl_result = ModalDialogBuilder::openButtonDialog(
                    declaratorType,
                    printOptions[1]
                );

                if (decl_result == -1) return;

                Print::DeclaratorType type = static_cast<Print::DeclaratorType>(decl_result);

                Print::printHirbNoDeclaration(*patient, type);

                return;
            }
        }

        case 2: Print::consent(*patient); return;

        case 3: Print::gdpr(*patient); return;
        default: return;
    }


}

void PatientInfoPresenter::setCurrent()
{
    if (
        !patient->insuranceStatus.has_value() &&
        User::practice().nzok_contract->nra_pass.size() &&
        User::settings().getNraStatusAuto
        )
    {
        nraClicked(false);
    }

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