#include "UpdateDialog.h"
#include "DbUpdates/DbUpdates.h"
#include <qapplication.h>

UpdateDialog::UpdateDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setModal(true);
	setWindowTitle("Обновяване на базата данни");
	//setWindowFlags(Qt::FramelessWindowHint);

}

void UpdateDialog::execute()
{
	open();
	 
	DbUpdates::backupDatabase();

	DbUpdates::update5();
	DbUpdates::update6(*this);
	DbUpdates::update7(*this);
	DbUpdates::update8(*this);
	DbUpdates::update9(*this);
	DbUpdates::update10(*this);
	DbUpdates::update11();
	close();
}

void UpdateDialog::setRange(int range)
{
	ui.progressBar->setRange(0, range);
	ui.progressBar->setValue(0);
	QCoreApplication::processEvents();

}

void UpdateDialog::increment()
{
	ui.progressBar->setValue(ui.progressBar->value() + 1);
	QCoreApplication::processEvents();
}

UpdateDialog::~UpdateDialog()
{}
