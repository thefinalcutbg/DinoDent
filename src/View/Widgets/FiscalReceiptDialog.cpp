#include "FiscalReceiptDialog.h"
#include "Presenter/FiscalReceiptPresenter.h"
#include <QPainter>


FiscalReceiptDialog::FiscalReceiptDialog(FiscalReceiptPresenter& p, QWidget *parent)
	: presenter(p), QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Потребителска такса пенсионери");

	connect(ui.okButton, &QPushButton::clicked, [&] { 
	
		if (!ui.fiscalNum->validateInput()) {
			ui.fiscalNum->setFocus();
			return;
		}

		if (!ui.deviceNum->validateInput()) {
			ui.deviceNum->setFocus();
			return;
		}

		presenter.okPressed(); 
		
	});

	presenter.setView(this);

	ui.deviceNum->setInputValidator(&notEmptyValidator);
	ui.fiscalNum->setInputValidator(&notEmptyValidator);

}

void FiscalReceiptDialog::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}

void FiscalReceiptDialog::setReceipt(const FiscalReceipt & r)
{
	ui.dateTimeEdit->setDateTime(QDateTime::fromString(r.timestamp.c_str(), Qt::ISODate));
	ui.deviceNum->setText(r.fiscal_memory.c_str());
	ui.fiscalNum->setText(r.receipt_num.c_str());
}

FiscalReceipt FiscalReceiptDialog::getReceipt()
{
	return FiscalReceipt{
		.timestamp = ui.dateTimeEdit->dateTime().toString(Qt::ISODate).toStdString(),
		.fiscal_memory = ui.deviceNum->text().toStdString(),
		.receipt_num = ui.fiscalNum->text().toStdString()
	};
}

FiscalReceiptDialog::~FiscalReceiptDialog()
{}
