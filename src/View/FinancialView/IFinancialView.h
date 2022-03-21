#pragma once

class FinancialPresenter;
struct Invoice;

class IFinancialView
{

public:
	virtual void setPresenter(FinancialPresenter* presenter) =0;
	virtual void setInvoice(const Invoice& inv) = 0;
};