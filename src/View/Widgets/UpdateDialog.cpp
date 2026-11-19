#include "UpdateDialog.h"
#include <qapplication.h>

UpdateDialog::UpdateDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowTitle("Обновяване на базата данни");
	setStyleSheet("QDialog { background-color: white; }");
}

void UpdateDialog::setLabelText(const QString& text)
{
	ui.label->setText(text);
	QCoreApplication::processEvents();
}

void UpdateDialog::setRange(int range)
{
	ui.progressBar->setRange(0, range);
	ui.progressBar->setValue(0);
	QCoreApplication::processEvents();
}

void UpdateDialog::increment()
{
	show();
	ui.progressBar->setValue(ui.progressBar->value() + 1);
	QCoreApplication::processEvents();
}

UpdateDialog::~UpdateDialog()
{}
