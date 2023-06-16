#pragma once

#include <vector>
#include "Model/Financial/FiscalReceipt.h"
#include "View/Interfaces/IFiscalReportView.h"

class FiscalReportPresenter
{
	IFiscalReportView* view{ nullptr };

	std::vector<FiscalReceipt> m_data;

	int m_month{ 1 };
	int m_year{ 2000 };

	std::string getFilename();

public:

	FiscalReportPresenter();

	void generateDescription();
	void generateInvoice();
	void deleteReceipt(int idx);
	void editReceipt(int idx);
	void setView(IFiscalReportView* v);
	void dateChanged(int month, int year);
};