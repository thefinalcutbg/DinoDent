#include "ErrorDetailedDialog.h"

ErrorDetailedDialog::ErrorDetailedDialog(const std::string& description, const std::string& details)
	: QDialog(nullptr)
{
	ui.setupUi(this);

	setWindowTitle("Възникна грешка");

	ui.detailsPane->hide();
	ui.detailsPane->setPlainText(details.c_str());
	ui.descriptionLabel->setText(description.c_str());

	connect(ui.detailsButton, &QPushButton::clicked, this,
		[&] {
			ui.detailsPane->setHidden(!ui.detailsPane->isHidden());
	});
}

ErrorDetailedDialog::~ErrorDetailedDialog()
{}
