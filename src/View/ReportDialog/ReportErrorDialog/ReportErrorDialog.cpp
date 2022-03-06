#include "ReportErrorDialog.h"

ReportErrorDialog::ReportErrorDialog(const std::string& errors, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowTitle(u8"Неуспешно генериране на месечен отчет");
	ui.errorTextBox->setReadOnly(true);
	ui.errorTextBox->setPlainText(QString::fromStdString(errors));

	connect(ui.okButton, &QPushButton::clicked, [=] {close();});
}

ReportErrorDialog::~ReportErrorDialog()
{
}
