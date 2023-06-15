#include "FiscalReportPresenter.h"
#include "Database/DbFiscalReceipt.h"
#include "View/ModalDialogBuilder.h"
#include "View/Printer.h"

FiscalReportPresenter::FiscalReportPresenter()
{}

void FiscalReportPresenter::generateDescription()
{
	if (m_data.empty()) {
		ModalDialogBuilder::showMessage("Няма описани касови бонове за избрания период");
	}

	FiscalReport r;
	
	auto dateFrom = Date{ 1, m_month, m_year };

	r.dateFrom = dateFrom.toBgStandard();
	r.dateTo = dateFrom.getMaxDateOfMonth().toBgStandard();
	r.receipts = m_data;

	Print::fiscalReport(r);
}

void FiscalReportPresenter::generateInvoice()
{
}

void FiscalReportPresenter::deleteReceipt(int idx)
{
	if (idx < 0 || idx >= m_data.size()) return;

	DbFiscalReceipt::deleteReceipt(m_data[idx].amblistRowid, m_data[idx].datetime);

	m_data = DbFiscalReceipt::getReceipts(m_month, m_year);

	if (view) view->setFiscalData(m_data);
}

void FiscalReportPresenter::editReceipt(int idx)
{
	if (idx < 0 || idx >= m_data.size()) return;
}

void FiscalReportPresenter::setView(IFiscalReportView* v)
{
	this->view = v;

	view->setFiscalData(m_data);
}

void FiscalReportPresenter::dateChanged(int month, int year)
{
	m_month = month;
	m_year = year;
	m_data = DbFiscalReceipt::getReceipts(month, year);

	if(view) view->setFiscalData(m_data);
}
