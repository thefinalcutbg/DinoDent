#pragma once

class FinancialPresenter;
#include "Model/Financial/Invoice.h"


class IFinancialView
{

public:
	virtual void setPresenter(FinancialPresenter* presenter) =0;
	virtual void setInvoice(const Invoice& inv) = 0;
    virtual void setBusinessOperations(const BusinessOperations& businessOp, double amount, bool hasVAT) = 0;
	virtual void setMainDocument(const std::optional<MainDocument>& mainDoc) = 0;
	virtual void setNumberSpinBox(long long num) = 0;
};
