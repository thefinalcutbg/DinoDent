#pragma once

#include <QWidget>
#include "ui_FiscalReportView.h"
#include "View/Interfaces/IFiscalReportView.h"
#include "View/TableModels/FiscalReceiptTableModel.h"

#include "Presenter/FiscalReportPresenter.h"

class FiscalReportView : public QWidget, public IFiscalReportView
{
	Q_OBJECT

	FiscalReportPresenter presenter;

	FiscalReceiptTableModel data_model;

	void paintEvent(QPaintEvent* e) override;

	int getCurrentIndex();

public:
	FiscalReportView(QWidget *parent = nullptr);
	~FiscalReportView();

	void setFiscalData(const std::vector<FiscalReceipt>& data) final;


private:


	Ui::FiscalReportView ui;



};
