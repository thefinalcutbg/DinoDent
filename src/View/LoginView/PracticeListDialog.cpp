#include "PracticeListDialog.h"

PracticeListDialog::PracticeListDialog(QDialog *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.setupUi(this);
	connect(ui.okButton, &QPushButton::clicked, this,
		[=] { done(ui.comboBox->currentIndex()); close(); });
}

int PracticeListDialog::exec(const std::vector<std::string>& practiceList)
{
	for (auto& practice : practiceList)
		ui.comboBox->addItem(QString::fromStdString(practice));

	return QDialog::exec();
}

PracticeListDialog::~PracticeListDialog()
{
}
