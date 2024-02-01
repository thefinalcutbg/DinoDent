#pragma once

#include <QDialog>
#include "ui_FiscalReceiptDialog.h"

#include "View/Interfaces/IFiscalReceiptDialog.h"
#include "Model/Validators/CommonValidators.h"

class FiscalReceiptPresenter;

class FiscalReceiptDialog : public QDialog, public IFiscalReceiptDialog
{
	Q_OBJECT

	NotEmptyValidator notEmptyValidator;

	FiscalReceiptPresenter& presenter;

	void paintEvent(QPaintEvent* e) override;

public:
    FiscalReceiptDialog(FiscalReceiptPresenter& p, QWidget *parent = nullptr);
	~FiscalReceiptDialog();

private:
	Ui::FiscalReceiptDialogClass ui;

	// Inherited via IFiscalReceiptDialog
	void setReceipt(const FiscalReceipt& r) final;
	FiscalReceipt getReceipt() final;
	void closeDialog() final { close(); }
};
