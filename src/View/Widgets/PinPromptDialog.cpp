﻿#include "PinPromptDialog.h"
#include <qsslcertificate.h>

PinPromptDialog::PinPromptDialog(const std::string& pem, const std::string& driver, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	QSslCertificate cert(pem.data());

	setWindowTitle("Вход с КЕП");

	ui.issuerLabel->setText(cert.issuerDisplayName());
	ui.nameLabel->setText(cert.subjectDisplayName());
	ui.expieryLabel->setText(cert.expiryDate().toString("dd.MM.yyyy") + " г.");
	ui.driverLabel->setText(driver.c_str());
	ui.lineEdit->setEchoMode(QLineEdit::EchoMode::Password);

    connect(ui.okButton, &QPushButton::clicked, this,
        [&] {
			auto pass = ui.lineEdit->text();
			if (pass.isEmpty()) {
				ui.lineEdit->setFocus();
				return;
			}

			result = pass;

			accept();

		});

    connect(ui.cancelButton, &QPushButton::clicked, this, [&]{reject();});

}

#include <QPainter>

void PinPromptDialog::paintEvent(QPaintEvent*)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	painter.end();
}

PinPromptDialog::~PinPromptDialog()
{
}
