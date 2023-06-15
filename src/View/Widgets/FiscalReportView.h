#pragma once

#include <QWidget>
#include "ui_FiscalReportView.h"
#include "View/Interfaces/IFiscalReportView.h"
#include "View/TableModels/FiscalReceiptTableModel.h"

#include "Presenter/FiscalReportPresenter.h"

class FiscalReportView : public QWidget, public IFiscalReportView
{
	Q_OBJECT

public:
	FiscalReportView(QWidget *parent = nullptr);
	~FiscalReportView();

private:

	void paintEvent(QPaintEvent* e) override;

	Ui::FiscalReportView ui;

	void setFiscalData(const std::vector<FiscalReceipt>& data) final;

	FiscalReportPresenter presenter;

	FiscalReceiptTableModel data_model;

	int getCurrentIndex();
};
