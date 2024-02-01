#include "FiscalReceiptDialog.h"
#include "Presenter/FiscalReceiptPresenter.h"
#include <QPainter>


FiscalReceiptDialog::FiscalReceiptDialog(FiscalReceiptPresenter& p, QWidget *parent)
    : QDialog(parent), presenter(p)
{
    ui.setupUi(this);

    setWindowTitle("Потребителска такса пенсионери");

    connect(ui.okButton, &QPushButton::clicked, this, [&] {

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

void FiscalReceiptDialog::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}

void FiscalReceiptDialog::setReceipt(const std::string& timestamp, const std::string& fiscalMemory)
{
    ui.dateTimeEdit->setDateTime(QDateTime::fromString(timestamp.c_str(), Qt::ISODate));
    ui.deviceNum->setText(fiscalMemory.c_str());
}

FiscalReceipt FiscalReceiptDialog::getReceipt()
{
    FiscalReceipt result;
    result.timestamp = ui.dateTimeEdit->dateTime().toString(Qt::ISODate).toStdString();
    result.fiscal_memory = ui.deviceNum->text().toStdString();
    result.receipt_num = ui.fiscalNum->text().toStdString();

    return result;
}

FiscalReceiptDialog::~FiscalReceiptDialog()
{}
