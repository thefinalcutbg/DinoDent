#include "DoctorDialogPresenter.h"
#include "View/uiComponents/AbstractLineEdit.h"
#include "Database/DbLogin.h"
#include "View/ModalDialogBuilder.h"

DoctorDialogPresenter::DoctorDialogPresenter(const Doctor& doctor) :
    result(doctor), current_LPK(doctor.LPK), view(nullptr)
{}

void DoctorDialogPresenter::okPressed()
{
    AbstractLineEdit* lineCheck[4]
    {
        view->lineEdit(DoctorFields::EGN),
        view->lineEdit(DoctorFields::FirstName),
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
    db.updateDoctor(doctor, current_LPK);

    result.emplace(doctor);

    view->close();
}

void DoctorDialogPresenter::setView(IDoctorSettingsDialog* view)
{
    this->view = view;

    view->lineEdit(DoctorFields::EGN)->setInputValidator(&egn_validator);
    view->lineEdit(DoctorFields::FirstName)->setInputValidator(&name_validator);
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
