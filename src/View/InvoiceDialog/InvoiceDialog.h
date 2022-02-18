#pragma once

#include <QDialog>
#include "ui_InvoiceDialog.h"
#include "dependencies/LimeReport/include/lrreportengine.h"
#include "IInvoiceDialog.h"

class InvoicePresenter;

class InvoiceDialog : public QDialog, public IInvoiceDialog
{
	Q_OBJECT

	InvoicePresenter* presenter;
	LimeReport::ReportEngine m_report;

public:
	InvoiceDialog(InvoicePresenter* presenter, QWidget *parent = Q_NULLPTR);
	void enableUserFields() final;
	void setInvoice(const Invoice& inv) final;
	void refreshNumberAndDate(const std::string& number, const Date& date) final;
	~InvoiceDialog();

private:
	Ui::InvoiceDialog ui;
};
