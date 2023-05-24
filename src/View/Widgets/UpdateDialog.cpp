#include "UpdateDialog.h"
#include "DbUpdates/DbUpdates.h"
#include <qapplication.h>

UpdateDialog::UpdateDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowTitle("Обновяване на базата данни");
	//setWindowFlags(Qt::FramelessWindowHint);

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
