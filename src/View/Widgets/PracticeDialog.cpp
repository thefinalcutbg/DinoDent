#include "PracticeDialog.h"
#include "Presenter/PracticeDialogPresenter.h"
#include <QPainter>

void PracticeDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}

PracticeDialog::PracticeDialog(PracticeDialogPresenter* presenter, QWidget *parent)
	: presenter(presenter), QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle(u8"Настройки на практиката");

	connect(ui.okButton, &QPushButton::clicked, [=] {presenter->okPressed();});
	presenter->setView(this);
}


IPracticeSettings* PracticeDialog::practiceSettingsView()
{
	return ui.practiceSettings;
}


void PracticeDialog::closeDialog()
{
	this->close();
}


PracticeDialog::~PracticeDialog()
{
}
