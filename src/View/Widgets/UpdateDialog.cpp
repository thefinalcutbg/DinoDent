#include "UpdateDialog.h"
#include "Updates/Updater.h"
#include <qapplication.h>
UpdateDialog::UpdateDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setModal(false);
	setWindowFlags(Qt::FramelessWindowHint);
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
