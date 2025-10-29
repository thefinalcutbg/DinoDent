#pragma once

#include <QDialog>
#include "ui_FiscalReceiptDialog.h"

#include "Model/Validators/CommonValidators.h"
#include "Model/Financial/FiscalReceipt.h"

class FiscalReceiptPresenter;

class FiscalReceiptDialog : public QDialog
{
	Q_OBJECT

	NotEmptyValidator notEmptyValidator;

	FiscalReceiptPresenter& presenter;

	void paintEvent(QPaintEvent* e) override;

public:
    FiscalReceiptDialog(FiscalReceiptPresenter& p, QWidget *parent = nullptr);

	void setReceipt(const std::string& timestamp, const std::string& fiscalMemory);
	FiscalReceipt getReceipt();

	~FiscalReceiptDialog();

private:
	Ui::FiscalReceiptDialogClass ui;
};
