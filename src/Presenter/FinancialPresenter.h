#pragma once
#include "Presenter/TabInstance.h"
#include "Model/Financial/Invoice.h"
#include "View/Interfaces/IFinancialView.h"
#include "Model/Dental/AmbList.h"
#include "Model/Patient.h"
#include "Network/PIS/SendFileService.h"


class FinancialPresenter : public TabInstance
{
	IFinancialView* view;

	SendFileService file_handler;

	// Inherited via TabInstance
public:

	Invoice m_invoice;

	FinancialPresenter(ITabView* tabView, const std::string& monthNotif, const std::string& claimedHash);
	FinancialPresenter(ITabView* tabView, std::shared_ptr<Patient> patient, const std::vector<Procedure>& procedures = {});
	FinancialPresenter(ITabView* tabView, long long rowId);
	FinancialPresenter(ITabView* tabView, const Recipient& r);

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
	void editRecipient();
	void editIssuer();
	void invoiceNumberChanged(long long number);
    void vatChanged(bool isVat);

	long long rowID() const override;
	bool save() override;
	void print() override;
	void pdfPrint() override;
	void setDataToView() override;
	bool isNew() override;
	TabName getTabName() override;
};

