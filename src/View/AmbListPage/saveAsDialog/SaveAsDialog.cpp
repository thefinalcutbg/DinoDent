#include "SaveAsDialog.h"

SaveAsDialog::SaveAsDialog(QWidget* parent)
    : QDialog(parent), currentNumber(0), presenter(nullptr)
{
    ui.setupUi(this);
    connect(ui.okButton, &QPushButton::clicked, this,
        [=] { okButtonPressed();});
    connect(ui.cancelButton, &QPushButton::clicked, this, [=] { reject(); });

    ui.ambNumSpin->setLabelMsg(ui.existLabel);
    ui.existLabel->setStyleSheet("color: red");
}


SaveAsDialog::~SaveAsDialog()
{

}

int SaveAsDialog::exec(std::map <int, bool>validNumber, int currentNumber)
{
    ui.ambNumSpin->setMap(&validNumber);
    ui.ambNumSpin->setValue(currentNumber);
    ui.ambNumSpin->setFocus();

    return QDialog::exec();
}


void SaveAsDialog::okButtonPressed()
{
    if (ui.ambNumSpin->isValid())
    {
        done(ui.ambNumSpin->value());
        close();
    }
    else
    {
        ui.ambNumSpin->setFocus();
        ui.ambNumSpin->selectAll();
    }

}

void SaveAsDialog::reject()
{
    done(0);
}



