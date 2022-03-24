#pragma once
#include "Presenter/TabPresenter/TabInstance.h"
#include "Model/Financial/Invoice.h"
#include "View/FinancialView/IFinancialView.h"
#include "Model/AmbList.h"
#include "Model/Patient.h"

class FinancialPresenter : public TabInstance
{
	Invoice m_invoice;
	
	IFinancialView* view;

	// Inherited via TabInstance
public:
	FinancialPresenter(ITabView* tabView, const std::string& monthNotifFilepath);
	FinancialPresenter(ITabView* tabView, const Patient& patient, const Procedures& procedures);

	void addDeleteOperation(int idx);
	void addOperation();
	void removeOperation(int idx);
	void dateChanged(Date date);
	void saveAsXML(const std::string& filePath);

	const std::string& rowID() const override;
	bool save() override;
	bool saveAs() override;
	void print() override;
	void setCurrent() override;
	bool isNew() override;
	TabName getTabName() override;
};

