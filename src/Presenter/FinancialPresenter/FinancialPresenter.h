#pragma once
#include "Presenter/TabPresenter/TabInstance.h"
#include "Model/Financial/Invoice.h"

class FinancialPresenter : public TabInstance
{
	Invoice m_invoice;

	// Inherited via TabInstance
public:
	virtual const std::string& rowID() const override;
	virtual bool save() override;
	virtual bool saveAs() override;
	virtual void print() override;
	virtual void setCurrent() override;
	virtual bool isNew() override;
	virtual TabName getTabName() override;
};

