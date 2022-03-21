#include "FinancialPresenter.h"

const std::string& FinancialPresenter::rowID() const
{
	return m_invoice.rowId;
}

bool FinancialPresenter::save()
{
	return false;
}

bool FinancialPresenter::saveAs()
{
	return false;
}

void FinancialPresenter::print()
{
}

void FinancialPresenter::setCurrent()
{
}

bool FinancialPresenter::isNew()
{
	return m_invoice.rowId.empty();
}

TabName FinancialPresenter::getTabName()
{
	return TabName{ u8"Финансов документ ", u8"№" + m_invoice.number };
}
