#pragma once

#include <QWidget>
#include "ui_FiscalReportView.h"

#include "View/TableModels/FiscalReceiptTableModel.h"

#include "Presenter/FiscalReportPresenter.h"

class FiscalReportView : public QWidget
{
	Q_OBJECT

	FiscalReportPresenter presenter;

	FiscalReceiptTableModel data_model;

	int getCurrentIndex();

public:
	FiscalReportView(QWidget *parent = nullptr);
	~FiscalReportView();

	void setFiscalData(const std::vector<FiscalReceipt>& data);

private:


	Ui::FiscalReportView ui;



};
