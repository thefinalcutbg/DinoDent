#include "FiscalReportPresenter.h"
#include "Database/DbFiscalReceipt.h"

FiscalReportPresenter::FiscalReportPresenter()
{}

void FiscalReportPresenter::generateDescription()
{
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
