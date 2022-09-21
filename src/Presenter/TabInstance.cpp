#include "TabInstance.h"

void TabInstance::makeEdited()
{
	if (edited) return;

	edited = true;
	auto tabName = getTabName();
	tabName.header = "*" + tabName.header;
	_tabView->changeTabName(tabName, m_containerIdx);
}

void TabInstance::refreshTabName()
{
	if (m_containerIdx == -1) return;

	_tabView->changeTabName(getTabName(), m_containerIdx);
}

bool TabInstance::close()
{
	if (!isNew() && !edited) return true;

	DialogAnswer answer = ModalDialogBuilder::openSaveDialog(getTabName().toString());

	switch (answer)
	{
	case DialogAnswer::Yes:
		//is save interrupted?
		return save() ? true : false;

	case DialogAnswer::No: return true;

	case DialogAnswer::Cancel: return false;
	}
}

void TabInstance::setCurrent()
{

	setDataToView();

	switch (type)
	{
	case TabType::AmbList: _tabView->showListView(); break;
	case TabType::PerioList: _tabView->showPerioView(); break;
	case TabType::PatientSummary: _tabView->showSummaryView(); break;
	case TabType::Financial:_tabView->showFinancialView(); break;
	case TabType::Prescription:_tabView->showPerscriptionView(); break;
	}

	setScrollPosition();
}

void TabInstance::prepareSwitch()
{
	m_scrollPos = _tabView->getScrollPos();
}
