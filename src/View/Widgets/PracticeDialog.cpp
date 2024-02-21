#include "PracticeDialog.h"
#include "Presenter/PracticeDialogPresenter.h"
#include <QPainter>

void PracticeDialog::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}

PracticeDialog::PracticeDialog(PracticeDialogPresenter& presenter, QWidget *parent)
    : QDialog(parent), presenter(presenter)
{
	ui.setupUi(this);

	setWindowTitle("Настройки на практиката");

    connect(ui.okButton, &QPushButton::clicked, this, [&] {presenter.okPressed();});
    presenter.setView(this);

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
