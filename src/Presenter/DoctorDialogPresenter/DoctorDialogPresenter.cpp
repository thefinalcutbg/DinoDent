#include "DoctorDialogPresenter.h"
#include "View/uiComponents/AbstractLineEdit.h"
#include "Database/DbLogin.h"
#include "View/ModalDialogBuilder.h"

DoctorDialogPresenter::DoctorDialogPresenter() : m_editMode(false), m_newDoctor(true)
{
}

DoctorDialogPresenter::DoctorDialogPresenter(const Doctor& doctor) :
    result(doctor), current_LPK(doctor.LPK), view(nullptr), m_editMode(true), m_newDoctor(false)
{}

void DoctorDialogPresenter::okPressed()
{
    AbstractLineEdit* lineCheck[5]
    {
        view->lineEdit(DoctorFields::EGN),
        view->lineEdit(DoctorFields::FirstName),
        view->lineEdit(DoctorFields::MiddleName),
        view->lineEdit(DoctorFields::LastName),
        view->lineEdit(DoctorFields::LPK)
    };

    for (auto& line : lineCheck)
    {
        line->validateInput();

        if (!line->isValid())
        {
            line->setFocus();
            return;
        }
    }

    auto doctor = view->getDoctor();

    DbLogin db;

    if (m_editMode){
        db.updateDoctor(doctor, current_LPK);
    }
    else if (m_newDoctor){
        db.insertDoctor(doctor);
        
    }


    result.emplace(doctor);

    view->close();
}

void DoctorDialogPresenter::validLPK(const std::string& validLPK)
{
    if (m_editMode) {
        return;
    }

    DbLogin db;
    auto doc = db.getDoctor(validLPK);

    if (doc.has_value())
    {
        m_newDoctor = false;
        view->setDoctor(doc.value());
        view->setToReadOnly();
    }
}

void DoctorDialogPresenter::setView(IDoctorSettingsDialog* view)
{
    this->view = view;

    view->lineEdit(DoctorFields::EGN)->setInputValidator(&egn_validator);
    view->lineEdit(DoctorFields::FirstName)->setInputValidator(&name_validator);
    view->lineEdit(DoctorFields::MiddleName)->setInputValidator(&name_validator);
    view->lineEdit(DoctorFields::LastName)->setInputValidator(&name_validator);
    view->lineEdit(DoctorFields::LPK)->setInputValidator(&lpk_validator);
    if (result.has_value()) {
        view->setDoctor(result.value());
        result.reset();
    }
        
}

std::optional<Doctor> DoctorDialogPresenter::open()
{
    ModalDialogBuilder::openDialog(this);
    
    return result;
}
