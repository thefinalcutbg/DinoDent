#include "PatientInfoPresenter.h"

#include "Database/DbPatient.h"
#include "Database/DbNotes.h"
#include "Database/DbNotification.h"

#include "Model/User.h"
#include "Model/TableRows.h"
#include "Model/FreeFunctions.h"

#include "Presenter/PatientDialogPresenter.h"
#include "Presenter/MedicalStatusPresenter.h"
#include "Presenter/TabPresenter.h"

#include "View/Widgets/NotificationDialog.h"
#include "View/SubWidgets/PatientTileInfo.h"
#include "View/ModalDialogBuilder.h"
#include "View/Widgets/MultilineDialog.h"

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

    nraStatusServ.sendRequest(
        *patient.get(),
        [=, this](auto status) { this->setInsuranceStatus(status);},
        showDialog
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
    MultilineDialog d(patient->patientNotes);
    d.setWindowTitle("Бележки за пациента (не се изпращат към НЗИС)");
    d.enableEditing();
    d.enableNotesFormat();
    auto result = d.getResult();

    if (result) {
        DbNotes::saveNote(result.value(), patient->rowid, -1);
        patient->patientNotes = result.value();
        view->setPatient(*patient, patientAge);
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

void PatientInfoPresenter::refreshPatientData()
{
	view->setPatient(*patient, patientAge);
}

void PatientInfoPresenter::sendSms()
{
	FreeFn::sendSMS(patient->rowid);
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
