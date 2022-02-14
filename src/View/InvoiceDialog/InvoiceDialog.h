#pragma once

#include <QDialog>
#include "ui_InvoiceDialog.h"

class InvoiceDialog : public QDialog
{
	Q_OBJECT

public:
	InvoiceDialog(QWidget *parent = Q_NULLPTR);
	~InvoiceDialog();

private:
	Ui::InvoiceDialog ui;
};
