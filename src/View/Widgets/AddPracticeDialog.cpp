#include "AddPracticeDialog.h"
#include "Presenter/AddPracticePresenter.h"
#include <QPainter>

void AddPracticeDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}

AddPracticeDialog::AddPracticeDialog(AddPracticePresenter* presenter, QWidget *parent)
	: presenter(presenter), QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle(u8"Добавяне на практика");

	connect(ui.okButton, &QPushButton::clicked, [=] {presenter->okPressed();});
	presenter->setView(this);
}


IPracticeSettings* AddPracticeDialog::practiceSettingsView()
{
	return ui.practiceSettings;
}

IPracticeDoctorSettings* AddPracticeDialog::doctorSettingsView()
{
	return ui.doctorSettings;
}

void AddPracticeDialog::closeDialog()
{
	this->close();
}


AddPracticeDialog::~AddPracticeDialog()
{
	
}
