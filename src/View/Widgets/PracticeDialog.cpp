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

    ui.practiceNameEdit->setErrorLabel(ui.errorLabel);
    ui.rziEdit->setErrorLabel(ui.errorLabel);
    ui.activityAddressEdit->setErrorLabel(ui.errorLabel);

    connect(ui.okButton, &QPushButton::clicked, this, [&] {

        LineEdit* fields[]{
            ui.practiceNameEdit,
            ui.rziEdit,
            ui.activityAddressEdit
        };

        for(auto field : fields)
        {
            field->validateInput();

            if(!field->isValid()) {
                field->set_focus();
                return;
            }
        }

        m_data = Data{
            .name = ui.practiceNameEdit->getText(),
            .rzi = ui.rziEdit->getText(),
            .address = ui.activityAddressEdit->getText(),
        };

        close();
    });


}

PracticeDialog::~PracticeDialog()
{
}
