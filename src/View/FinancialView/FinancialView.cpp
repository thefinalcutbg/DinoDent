#include "FinancialView.h"
#include "Model/Financial/Invoice.h"
FinancialView::FinancialView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

FinancialView::~FinancialView()
{
}

void FinancialView::setPresenter(FinancialPresenter* presenter)
{
	this->presenter = presenter;
}

void FinancialView::setInvoice(const Invoice& inv)
{
	ui.issuerButton->setIssuer(inv.issuer);
	ui.recipientButton->setRecipient(inv.recipient);
	ui.dateEdit->setDate(QDate{ inv.date.year, inv.date.month, inv.date.day });

}
