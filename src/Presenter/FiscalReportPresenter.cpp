#include "FiscalReportPresenter.h"
#include "Database/DbFiscalReceipt.h"

#include "Model/Financial/Invoice.h"
#include "Model/User.h"
#include "Model/FreeFunctions.h"

#include "View/ModalDialogBuilder.h"
#include "Printer/Print.h"

#include "View/Widgets/FiscalReportView.h"

std::string FiscalReportPresenter::getFilename()
{
	std::string filename = "DENT_";
	filename += User::practice().rziCode;
	filename += "_";
	filename += FreeFn::leadZeroes(m_month, 2);
	filename += std::to_string(m_year);
	return filename;
}

FiscalReportPresenter::FiscalReportPresenter()
{}

void FiscalReportPresenter::generateDescription()
{
	if (m_data.empty()) {
		ModalDialogBuilder::showMessage("Няма описани касови бонове за избрания период");
		return;
	}

	ModalDialogBuilder::showMessage(
		"Запазете документа, подпишете го електронно и го изпратете от сайта на ПИС"
	);

	auto filepath = ModalDialogBuilder::getFilePath(getFilename());
	if (!filepath) return;

	FiscalReport r;
	
	auto dateFrom = Date{ 1, m_month, m_year };

	r.filename = filepath.value();
	r.dateFrom = dateFrom.toBgStandard();
	r.dateTo = dateFrom.getMaxDateOfMonth().toBgStandard();
	r.receipts = m_data;

	Print::saveFsicalReportToPDF(r);
}

void FiscalReportPresenter::generateInvoice()
{
	
}

void FiscalReportPresenter::deleteReceipt(int idx)
{
	if (idx < 0 || idx >= m_data.size()) return;

	DbFiscalReceipt::deleteReceipt(m_data[idx].amblistRowid, m_data[idx].timestamp);

	m_data = DbFiscalReceipt::getReceipts(m_month, m_year);

	if (view) view->setFiscalData(m_data);
}

void FiscalReportPresenter::editReceipt(int idx)
{
	if (idx < 0 || idx >= m_data.size()) return;
}

void FiscalReportPresenter::setView(FiscalReportView* v)
{
	this->view = v;

	view->setFiscalData(m_data);
}

void FiscalReportPresenter::dateChanged(int month, int year)
{
	m_month = month;
	m_year = year;
	m_data = DbFiscalReceipt::getReceipts(month, year);

	if (view == nullptr) return;
	
	view->setFiscalData(m_data);
}
