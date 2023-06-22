#include "MultilineDialog.h"

MultilineDialog::MultilineDialog(const std::string& errors, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.errorTextBox->setReadOnly(true);
	ui.errorTextBox->setPlainText(QString::fromStdString(errors));

	connect(ui.okButton, &QPushButton::clicked, [=] {close();});
}

MultilineDialog::~MultilineDialog()
{
}
