#include "PatientInfoPresenter.h"
#include "Presenter/PatientDialogPresenter.h"

#include "Database/DbPatient.h"
#include "View/ModalDialogBuilder.h"
#include "View/Printer.h"

PatientInfoPresenter::PatientInfoPresenter(IPatientTileInfo* view, std::shared_ptr<Patient> p) :
    view(view), patient(p), patientAge(patient->getAge(Date::currentDate()))
{}

void PatientInfoPresenter::setDate(const Date& date)
{
    patientAge = patient->getAge(date);

    if (!m_isCurrent) return;

    view->setPatient(*patient, patientAge);
}

void PatientInfoPresenter::nraClicked(bool showDialog)
{
    m_showInsuranceDialog = showDialog;

    /*
    if (patient->insuranceStatus) {
        setInsuranceStatus(patient->insuranceStatus.value());
        return;
    }
    */
    nraStatusServ.sendRequest(
        *patient.get(),
        [=](auto status) { this->setInsuranceStatus(status);},
        showDialog
    );
}

void PatientInfoPresenter::diagnosisClicked()
{
    nhifDiagnosisServ.sendRequest(
        *patient,


        [=](const std::vector<MedicalStatus>& currentDiseases) {

            if (currentDiseases.empty()) {
                ModalDialogBuilder::showMessage("Няма данни в рецептурната книжка");
                return;
            }

            MedicalStatuses medStats = this->patient->medStats;
            medStats.condition = currentDiseases;
            
            auto result = ModalDialogBuilder::openMedicalStatusDialog(medStats);

            if (!result.has_value()) { return; }

            DbPatient::updateMedStats(patient->rowid, result.value());

            patient->medStats = result.value();

            view->setPatient(*patient, patientAge);
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

    view->setPatient(*patient, patientAge);
}

void PatientInfoPresenter::allergiesTileClicked()
{
    auto result = ModalDialogBuilder::openMedicalStatusDialog(patient->medStats);

    if (!result) {
        return;
    }

    patient->medStats = result.value();

    auto success = DbPatient::updateMedStats(patient->rowid, patient->medStats);

    if (!success) return;

    view->setPatient(*patient, patientAge);
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

void PatientInfoPresenter::setCurrent(bool isCurrent)
{
    m_isCurrent = isCurrent;

    if (!isCurrent) return;

    view->setPresenter(this);

    view->setPatient(*patient, patientAge);

    if (
        !patient->insuranceStatus &&
        User::hasNhifContract() &&
        User::practice().nhif_contract->nra_pass.size() &&
        User::settings().getNraStatusAuto
    )
    {
        nraClicked(false);
    }


}

void PatientInfoPresenter::setInsuranceStatus(const std::optional<InsuranceStatus>& status_result)
{
    if (!status_result) {
        return;
    }

    patient->insuranceStatus = status_result.value();

    if (m_isCurrent) {
        view->setPatient(*patient, patientAge);
    }

    if (m_showInsuranceDialog) {
        m_showInsuranceDialog = false;
        ModalDialogBuilder::showMessage(status_result->getYearsText());
    }
}