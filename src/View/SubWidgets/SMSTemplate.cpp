#include "SMSTemplate.h"

#include <QDateTime>

#include "View/CommonIcon.h"
#include "Network/SMS/SMSMessage.h"
#include "View/ModalDialogBuilder.h"


SMSTemplate::SMSTemplate(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::SMSTemplateClass())
{
	ui->setupUi(this);

	ui->resetButton->setIcon(CommonIcon::getPixmap(CommonIcon::REFRESH));

	connect(ui->resetButton, &QPushButton::clicked, this, [&]() {
		setMessageTemplate(m_defaultTemplate);
	});

	connect(ui->lineEdit, &QLineEdit::textChanged, this, [&]() {

		QString previewText = SMSMessage(
			"",
			ui->lineEdit->text().toStdString(),
			"2025-12-08T10:00"
		).message.c_str();

		auto count = previewText.size();

		QString charText = "Дължина: ";
		charText += QString::number(count);
		charText += count == 1 ? " символ" : " символа";

        ui->charCountLabel->setStyleSheet(count < 70 ? "" : "color: darkred");

		ui->charCountLabel->setText(charText);
	});

	connect(ui->previewButton, &QPushButton::clicked, this, [&]() {

		QString previewText = SMSMessage(
			"", 
			ui->lineEdit->text().toStdString(),
			"2025-12-08T10:00"
			).message.c_str();
		
		ModalDialogBuilder::showMultilineDialog(
			previewText.toStdString(),
            "Преглед на съобщението"
		);
	});
}

void SMSTemplate::setDefaultMessageTemplate(const QString& text)
{
	m_defaultTemplate = text;
	if(ui->lineEdit->text().isEmpty()) {
		setMessageTemplate(m_defaultTemplate);
	}
}

void SMSTemplate::setMessageTemplate(const QString& text)
{
	ui->lineEdit->setText(text.isEmpty() ? m_defaultTemplate : text);
}

void SMSTemplate::setTitle(const QString& title)
{
	ui->groupBox->setTitle(title);
}

void SMSTemplate::setAutoCheck(bool autoCheck)
{
	ui->autoCheck->setChecked(autoCheck);
}

bool SMSTemplate::getAutoCheck()
{
	return ui->autoCheck->isChecked();
}

QString SMSTemplate::getMessageTemplate() const
{
	return ui->lineEdit ->text();
}

SMSTemplate::~SMSTemplate()
{
	delete ui;
}

