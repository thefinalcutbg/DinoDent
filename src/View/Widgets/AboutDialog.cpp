#include "AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setStyleSheet("background-color:white");
}

AboutDialog::~AboutDialog()
{}
