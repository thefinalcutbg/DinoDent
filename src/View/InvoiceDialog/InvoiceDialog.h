#pragma once

#include <QDialog>
#include "ui_InvoiceDialog.h"

#include "IInvoiceDialog.h"

class InvoicePresenter;

class InvoiceDialog : public QDialog, public IInvoiceDialog
{
	Q_OBJECT

	InvoicePresenter* presenter;

public:
	InvoiceDialog(InvoicePresenter* presenter, QWidget *parent = Q_NULLPTR);
	void enableUserFields() final;
	~InvoiceDialog();

private:
	Ui::InvoiceDialog ui;
};
