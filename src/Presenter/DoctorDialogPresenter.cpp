#include "DoctorDialogPresenter.h"
#include "View/Interfaces/AbstractLineEdit.h"
#include "Database/DbDoctor.h"
#include "View/ModalDialogBuilder.h"

DoctorDialogPresenter::DoctorDialogPresenter() : m_editMode(false), m_newDoctor(true)
{
}

DoctorDialogPresenter::DoctorDialogPresenter(const Doctor& doctor) :
    result(doctor), 
    current_LPK(doctor.LPK), 
    view(nullptr), 
    m_editMode(true), 
    m_newDoctor(false),
    nhif_specialty{ doctor.specialty }
{}

void DoctorDialogPresenter::okPressed()
{
    AbstractLineEdit* lineCheck[6]
    {
        view->lineEdit(DoctorFields::FirstName),
        view->lineEdit(DoctorFields::MiddleName),
        view->lineEdit(DoctorFields::LastName),
        view->lineEdit(DoctorFields::LPK),
        view->lineEdit(DoctorFields::Phone),
        view->lineEdit(DoctorFields::Password)
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

    if (!doctor.hisSpecialty.isValid()) {
        ModalDialogBuilder::showMessage("Изберете специалност!");
        return;
    }

    if (m_editMode){
        DbDoctor::updateDoctor(doctor, current_LPK);
    }
    else if (m_newDoctor){
        DbDoctor::insertDoctor(doctor);
        
    }


    result.emplace(doctor);
    result->specialty = nhif_specialty;

    view->close();
}

void DoctorDialogPresenter::validLPK(const std::string& validLPK)
{
    if (m_editMode) {
        return;
    }

    auto doc = DbDoctor::getDoctor(validLPK);

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

    view->lineEdit(DoctorFields::FirstName)->setInputValidator(&name_validator);
    view->lineEdit(DoctorFields::MiddleName)->setInputValidator(&name_validator);
    view->lineEdit(DoctorFields::LastName)->setInputValidator(&name_validator);
    view->lineEdit(DoctorFields::LPK)->setInputValidator(&lpk_validator);
    view->lineEdit(DoctorFields::Phone)->setInputValidator(&not_emptyValidator);
   // view->lineEdit(DoctorFields::Password)->setInputValidator(&not_emptyValidator);
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
