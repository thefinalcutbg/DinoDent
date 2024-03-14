#include "PracticeDialog.h"
#include <QPainter>
#include "Model/Ekatte.h"

void PracticeDialog::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}

PracticeDialog::PracticeDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

    setWindowTitle("Добавяне на нова практика");

    ui.practiceNameEdit->setInputValidator(&not_empty_validator);
    ui.rziEdit->setInputValidator(&rzi_validator);
    ui.activityAddressEdit->setInputValidator(&city_validator);
    ui.activityAddressEdit->setCompletions(Ekatte::cityNameToIdx());
    ui.lpkEdit->setInputValidator(&lpk_validator);

    ui.practiceNameEdit->setErrorLabel(ui.errorLabel);
    ui.rziEdit->setErrorLabel(ui.errorLabel);
    ui.activityAddressEdit->setErrorLabel(ui.errorLabel);
    ui.lpkEdit->setErrorLabel(ui.errorLabel);

    connect(ui.okButton, &QPushButton::clicked, this, [&] {

        LineEdit* fields[4]{
            ui.practiceNameEdit,
            ui.rziEdit,
            ui.activityAddressEdit,
            ui.lpkEdit
        };

        for(int i = 0; i < 4; i++)
        {
            fields[i]->validateInput();

            if(!fields[i]->isValid()) {
                fields[i]->set_focus();
                return;
            }
        }

        m_data = Data{
            .name = ui.practiceNameEdit->getText(),
            .rzi = ui.rziEdit->getText(),
            .address = ui.activityAddressEdit->getText(),
            .lpk = ui.lpkEdit->getText()
        };

        close();
    });


}

PracticeDialog::~PracticeDialog()
{
}
