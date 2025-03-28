#pragma once

#include <QWidget>
#include "ui_FinancialView.h"
#include "Presenter/FinancialPresenter.h"
#include "View/Interfaces/IFinancialView.h"
#include "View/TableModels/BusinessOperationModel.h"

class FinancialView : public QWidget, public IFinancialView
{
	Q_OBJECT

	FinancialPresenter* presenter{nullptr};

	BusinessOperationModel m_model;

    void paintEvent(QPaintEvent* event) override;

    void showMainDocumentDetails(bool show);

public:
	FinancialView(QWidget *parent = Q_NULLPTR);

	void setPresenter(FinancialPresenter* presenter) override;
	void setInvoice(const Invoice& inv) override;
    void setBusinessOperations(const BusinessOperations& businessOp, double amount, bool hasVat) override;
    void setMainDocument(const std::optional<MainDocument>& mainDoc) override;
	void setNumberSpinBox(long long num) override;

	~FinancialView();

private:
	Ui::FinancialView ui;

	// Inherited via IFinancialView
};
