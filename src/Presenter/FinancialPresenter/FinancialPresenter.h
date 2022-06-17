#pragma once
#include "Presenter/TabPresenter/TabInstance.h"
#include "Model/Financial/Invoice.h"
#include "View/FinancialView/IFinancialView.h"
#include "Model/AmbList.h"
#include "Model/Patient.h"

class FinancialPresenter : public TabInstance
{
	IFinancialView* view;

	// Inherited via TabInstance
public:

	Invoice m_invoice;

	FinancialPresenter(ITabView* tabView, const std::string& monthNotif);
	FinancialPresenter(ITabView* tabView, const Procedures& procedures, std::shared_ptr<Patient> patient);
	FinancialPresenter(ITabView* tabView, long long rowId);

	void addOperation();
	void editOperation(int idx);
	void removeOperation(int idx);

	void dateChanged(Date date);
	void taxEventDateChanged(Date date);
	void paymentTypeChanged(PaymentType type);
	void saveAsXML();
	void sendToPis();
	void docTypeChanged(int index);
	void mainDocumentChanged(long long num, Date date);

	long long rowID() const override;
	bool save() override;
	bool saveAs() override;
	void print() override;
	void setCurrent() override;
	bool isNew() override;
	TabName getTabName() override;
};

