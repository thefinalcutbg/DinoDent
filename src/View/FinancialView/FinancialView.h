#pragma once

#include <QWidget>
#include "ui_FinancialView.h"
#include "Presenter/FinancialPresenter/FinancialPresenter.h"
#include "IFinancialView.h"
#include "BusinessOperationModel.h"

class FinancialView : public QWidget, public IFinancialView
{
	Q_OBJECT

	FinancialPresenter* presenter{nullptr};

	BusinessOperationModel m_model;

	void paintEvent(QPaintEvent* event);

public:
	FinancialView(QWidget *parent = Q_NULLPTR);

	void setPresenter(FinancialPresenter* presenter) override;
	void setInvoice(const Invoice& inv) override;
	void setBusinessOperations(const BusinessOperations& businessOp, const AggregatedAmounts& amounts) override;

	~FinancialView();

private:
	Ui::FinancialView ui;

	// Inherited via IFinancialView
};