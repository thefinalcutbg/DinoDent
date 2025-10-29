#include "PatientInfoPresenter.h"

#include "Database/DbPatient.h"
#include "Database/DbNotes.h"
#include "Database/DbNotification.h"

#include "Model/User.h"
#include "Model/TableRows.h"

#include "Presenter/PatientDialogPresenter.h"
#include "Presenter/MedicalStatusPresenter.h"
#include "Presenter/TabPresenter.h"

#include "View/Widgets/NotificationDialog.h"
#include "View/SubWidgets/PatientTileInfo.h"
#include "View/ModalDialogBuilder.h"

PatientInfoPresenter::PatientInfoPresenter(PatientTileInfo* view, std::shared_ptr<Patient> p) :
    patient(p), view(view), patientAge(patient->getAge(Date::currentDate()))
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
        [=, this](auto status) { this->setInsuranceStatus(status);},
        showDialog
    );
}

void PatientInfoPresenter::diagnosisClicked()
{
    nhifDiagnosisServ.sendRequest(
        *patient,


        [=, this](
            const std::vector<ICD10>& currentDiseases,
            const std::vector<ICD10>& pastDiseases
            
        ) {

            if (currentDiseases.empty() && pastDiseases.empty()) {
                ModalDialogBuilder::showMessage("Няма данни в рецептурната книжка");
                return;
            }
            
            auto& current = patient->medStats.condition;

            current.insert(current.begin(), currentDiseases.begin(), currentDiseases.end());

            auto& past = patient->medStats.history;

            past.insert(past.begin(), pastDiseases.begin(), pastDiseases.end());

            DbPatient::updateMedStatus(patient->rowid, patient->medStats);

            MedicalStatusPresenter p(*patient);

            p.openDialog();

            if(m_isCurrent) view->setPatient(*patient, patientAge);
        }
    );
}

void PatientInfoPresenter::allergiesClicked()
{
    allergyFetchServ.sendRequest(*patient, User::practice().rziCode,

        [&](const std::vector<Allergy> allergies) {

            if (allergies.empty()) {
                ModalDialogBuilder::showMessage("Няма данни за алергии в НЗИС");
                return;
            }

            std::vector<Allergy>::iterator it;

            it = std::remove_if(
                patient->allergies.begin(),
                patient->allergies.end(),
                [](const Allergy& a) {
                    return !a.nrn.empty();
                }
            );

            patient->allergies.erase(it, patient->allergies.end());

            for (auto& a : allergies) {

                if (a.verificationStatus == Allergy::EnteredInError) continue;

                patient->allergies.insert(patient->allergies.begin(), a);
            }

            DbPatient::updateAllergies(patient->rowid, patient->allergies);

            MedicalStatusPresenter p(*patient);

            p.openDialog();

            if (m_isCurrent) view->setPatient(*patient, patientAge);
            
        }
    );
}

void PatientInfoPresenter::patientTileClicked()
{
    if (patient == nullptr) return;

    PatientDialogPresenter p{ *patient };

    auto patient = p.getPatient();

    if (!patient.has_value()) return;

    *this->patient = patient.value();

    TabPresenter::get().refreshPatientTabNames(patient->rowid);

    view->setPatient(*patient, patientAge);

    if (m_parent) {
        m_parent->patientDataChanged();
    }
}

void PatientInfoPresenter::medStatTileClicked()
{
    if (nhifDiagnosisServ.awaitingReply() || allergyFetchServ.awaitingReply())
    {
        ModalDialogBuilder::showMessage("Моля изчакайте, очаква се отговор от сървъра");
        return;
    }

    MedicalStatusPresenter p(*patient);

    p.openDialog();

    view->setPatient(*patient, patientAge);

    if (m_parent) {
        m_parent->patientDataChanged();
    }
}

void PatientInfoPresenter::appointmentClicked()
{
    TabPresenter::get().openCalendar(CalendarEvent(*patient));
}

void PatientInfoPresenter::notesRequested()
{
    auto result = ModalDialogBuilder::showMultilineDialog(
        patient->patientNotes,
        "Бележки за пациента (не се изпращат към НЗИС)",
        true
    );

    if (result) {
        DbNotes::saveNote(result.value(), patient->rowid, -1);
        patient->patientNotes = result.value();
        view->setPatient(*patient, patientAge);
    }
}

void PatientInfoPresenter::checkHospitalization()
{
    eHospitalizationFetch.sendRequest(
        *patient,
        User::practice().rziCode,
        [](const std::vector<Hospitalization> list) {

            bool active = false;

            for (auto& h : list) {
                if (h.status == Hospitalization::Active) {
                    active = true;
                    break;
                }
            }

            ModalDialogBuilder::showMessage(
                active ?
                "В момента пациентът е с активна хоспитализация!"
                :
                "Не е открита активна хоспитализация"
            );
        }
    );
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

void PatientInfoPresenter::notificationClicked()
{
    NotificationDialog d;
    d.exec();

    auto result = d.getResult();

    if(!result) return;

    result->patientRowid = patient->rowid;

    DbNotification::insert(result.value());
}

void PatientInfoPresenter::openDocument(TabType type)
{
    RowInstance doc(type);
    doc.rowID = 0,
    doc.patientRowId = patient->rowid,

    TabPresenter::get().open(doc, true);
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
