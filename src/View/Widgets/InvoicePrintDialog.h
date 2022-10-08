#pragma once

#include <QDialog>
#include "ui_InvoicePrintDialog.h"

class InvoicePrintDialog : public QDialog
{
	Q_OBJECT

public:
	enum Result { Original, Duplicate, Canceled };
private:
	Result m_result{ Canceled };

	void paintEvent(QPaintEvent* event);

public:

	InvoicePrintDialog(QWidget *parent = nullptr);
	Result result() { return m_result; };
	~InvoicePrintDialog();

private:
	Ui::InvoicePrintDialogClass ui;
};
