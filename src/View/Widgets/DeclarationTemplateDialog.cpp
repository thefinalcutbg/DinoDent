#include "DeclarationTemplateDialog.h"
#include "View/CommonIcon.h"
#include "View/ModalDialogBuilder.h"
#include "Printer/Print.h"

DeclarationTemplateDialog::DeclarationTemplateDialog(const DeclarationTemplate& d)
	: QDialog(nullptr), m_rowid(d.rowid)
{
	ui.setupUi(this);

	setWindowTitle(d.rowid ? "Редакция на декларация" : "Добавяне на декларация");

	ui.okButton->setFocus();

	ui.previewButton->setIcon(QIcon(":/icons/icon_print.png"));

	std::pair<QPushButton*, QString> tagButtons[] = {
		{ ui.patientName, DeclarationTemplate::nametag},
		{ ui.patientId, DeclarationTemplate::idtag},
		{ ui.patientAddress, DeclarationTemplate::addresstag},
		{ ui.patientPhone, DeclarationTemplate::phonetag},
		{ ui.patientEmail, DeclarationTemplate::emailtag},
		{ ui.patientBirth, DeclarationTemplate::birthtag},
		{ ui.currentDate, DeclarationTemplate::currentdatetag},
		{ ui.doctorName, DeclarationTemplate::doctorNametag},
		{ ui.dentistLPK, DeclarationTemplate::dentistLPKtag},
		{ ui.dentistPhone, DeclarationTemplate::dentistPhonetag},
		{ ui.dentistSpecialty, DeclarationTemplate::dentistSpecialtytag}
	};

	for(auto& [button, tag] : tagButtons)
	{
		button->setIcon(QIcon(":/icons/icon_add.png"));
		button->setFocusPolicy(Qt::NoFocus);
		connect(button, &QPushButton::clicked, this, [this, tag]() {insertTag(tag);});
	}

	if(d.rowid != 0) {
		ui.headerEdit->setText(d.header.c_str());
		ui.subheaderEdit->setText(d.subheader.c_str());
		ui.bodyEdit->setPlainText(d.body.c_str());
		ui.footerEdit->setText(d.footer.c_str());
	}

	connect(ui.previewButton, &QPushButton::clicked, this, [this]() {

		auto preview = DeclarationTemplate{
			.rowid = m_rowid,
			.header = ui.headerEdit->text().toStdString(),
			.subheader = ui.subheaderEdit->text().toStdString(),
			.body = ui.bodyEdit->toPlainText().toStdString(),
			.footer = ui.footerEdit->text().toStdString()
		};

		if (!checkForm()) return;

		Print::previewDeclarationTemplate(preview);
	});

	connect(ui.okButton, &QPushButton::clicked, this, [this]() {

		if (!checkForm()) return;

		m_result = DeclarationTemplate{
			.rowid = m_rowid,
			.header = ui.headerEdit->text().toStdString(),
			.subheader = ui.subheaderEdit->text().toStdString(),
			.body = ui.bodyEdit->toPlainText().toStdString(),
			.footer = ui.footerEdit->text().toStdString()
		};

		accept();
	});
	
}

void DeclarationTemplateDialog::insertTag(const QString& tag)
{
    QWidget* widget = QApplication::focusWidget();

    if (auto* lineEdit = qobject_cast<QLineEdit*>(widget)) {
        lineEdit->insert(tag);
    }
    else if (auto* plainTextEdit = qobject_cast<QPlainTextEdit*>(widget)) {
        plainTextEdit->insertPlainText(tag);
    }
}
bool DeclarationTemplateDialog::checkForm()
{
	if(ui.headerEdit->text().isEmpty()) {
		ModalDialogBuilder::showMessage("Заглавието на декларацията не може да бъде празно.");
		ui.headerEdit->setFocus();
		return false;
	}

	if(ui.bodyEdit->toPlainText().isEmpty()) {
		ModalDialogBuilder::showMessage("Съдържанието на декларацията не може да бъде празно.");
		ui.bodyEdit->setFocus();
		return false;
	}

	return true;
}
DeclarationTemplateDialog::~DeclarationTemplateDialog()
{}

