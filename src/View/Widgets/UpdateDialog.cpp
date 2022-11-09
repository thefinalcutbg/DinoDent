#include "UpdateDialog.h"
#include "Updates/DbUpdates.h"
#include <qapplication.h>

UpdateDialog::UpdateDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setModal(true);
	setWindowTitle(u8"Обновяване на базата данни");
	//setWindowFlags(Qt::FramelessWindowHint);

}

void UpdateDialog::execute()
{
	open();

	/*
	DbUpdates::update1(this);
	DbUpdates::update2(this);
	DbUpdates::update3();
	DbUpdates::update4();
	*/
	DbUpdates::update5();
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
