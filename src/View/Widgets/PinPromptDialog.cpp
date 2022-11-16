#include "PinPromptDialog.h"
#include <qsslcertificate.h>
PinPromptDialog::PinPromptDialog(const std::string& pem, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	QSslCertificate cert(pem.data());

	setWindowTitle("Вход с КЕП");

	ui.issuerLabel->setText(cert.issuerDisplayName());
	ui.nameLabel->setText(cert.subjectDisplayName());

	ui.lineEdit->setEchoMode(QLineEdit::EchoMode::Password);
	ui.lineEdit->setMaxLength(10);

	connect(ui.okButton, &QPushButton::clicked,
		[=] {
			auto pass = ui.lineEdit->text();
			if (pass.isEmpty()) {
				ui.lineEdit->setFocus();
				return;
			}

			result = pass;

			accept();

		});

	connect(ui.cancelButton, &QPushButton::clicked, [=] {reject();});

}

#include <QPainter>

void PinPromptDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	painter.end();
}

PinPromptDialog::~PinPromptDialog()
{
}
