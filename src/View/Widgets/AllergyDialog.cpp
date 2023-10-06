#include "AllergyDialog.h"

AllergyDialog::AllergyDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Алергия");
}

AllergyDialog::~AllergyDialog()
{}
