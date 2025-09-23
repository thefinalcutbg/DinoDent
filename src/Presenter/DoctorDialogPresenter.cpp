#include "DoctorDialogPresenter.h"
#include "View/Interfaces/AbstractLineEdit.h"
#include "Database/DbDoctor.h"
#include "View/ModalDialogBuilder.h"

DoctorDialogPresenter::DoctorDialogPresenter(){}

DoctorDialogPresenter::DoctorDialogPresenter(const Doctor& doctor) :
    view(nullptr),
    current_LPK(doctor.LPK),
    result(doctor),
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
            line->set_focus();
            return;
        }
    }

    auto doctor = view->getDoctor();

    if (!doctor.hisSpecialty.isValid()) {
        ModalDialogBuilder::showMessage("Изберете специалност!");
        return;
    }

    if (current_LPK.size()){
        DbDoctor::updateDoctor(doctor, current_LPK);
    }
    else{
        DbDoctor::insertDoctor(doctor);
        
    }


    result.emplace(doctor);
    result->specialty = nhif_specialty;

    view->close();
}

void DoctorDialogPresenter::validLPK(const std::string& validLPK)
{
    if (current_LPK.size()) {
        return;
    }

    auto doc = DbDoctor::getDoctor(validLPK);

    if (doc.has_value())
    {
        current_LPK = validLPK;
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
    ModalDialogBuilder::openDialog(*this);
    
    return result;
}
