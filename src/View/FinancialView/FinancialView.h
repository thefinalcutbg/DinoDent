#pragma once

#include <QWidget>
#include "ui_FinancialView.h"
#include "Presenter/FinancialPresenter/FinancialPresenter.h"
#include "IFinancialView.h"

class FinancialView : public QWidget, public IFinancialView
{
	Q_OBJECT

	FinancialPresenter* presenter{nullptr};

public:
	FinancialView(QWidget *parent = Q_NULLPTR);
	~FinancialView();

private:
	Ui::FinancialView ui;

	// Inherited via IFinancialView
	virtual void setPresenter(FinancialPresenter* presenter) override;
	virtual void setInvoice(const Invoice& inv) override;
};
