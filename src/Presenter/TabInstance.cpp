#include "TabInstance.h"
#include "View/Widgets/TabView.h"
#include <QTimer>

void TabInstance::makeEdited()
{
	//if (edited) return;

	edited = true;
	auto tabName = getTabName();
	tabName.header = "*" + tabName.header;
	_tabView->changeTabName(tabName, m_containerIdx);
}

void TabInstance::setScrollPosition()
{
	_tabView->setScrollPos(m_scrollPos);
}

void TabInstance::refreshTabName()
{
	if (m_containerIdx == -1) return;

	_tabView->changeTabName(getTabName(), m_containerIdx);
}

bool TabInstance::premissionToClose()
{
	if (!isNew() && !edited) return true;

	std::string question = "Желаете ли да запазите промените по ";
	question += getTabName().toString();
	question += "?";

	DialogAnswer answer = ModalDialogBuilder::YesNoCancelDailog(question);

	switch (answer)
	{
		case DialogAnswer::Yes: return save(); //is save interrupted?
		case DialogAnswer::No: return true;
		case DialogAnswer::Cancel: return false;
	}

	return true;
}

void TabInstance::setCurrent()
{
	is_current = true;

	_tabView->disableViewportUpdates();

	setDataToView();
	
	QTimer::singleShot(0, [this] {
		switch (type) {
		case TabType::AmbList: _tabView->showListView(); break;
		case TabType::PerioStatus: _tabView->showPerioView(); break;
		case TabType::Financial: _tabView->showFinancialView(); break;
		case TabType::TreatmentPlan: _tabView->showTreatmentPlanView(); break;
		case TabType::Prescription: _tabView->showPerscriptionView(); break;
		case TabType::Calendar: _tabView->showCalendarView(); break;
		default: break;
		}

		setScrollPosition();
		_tabView->enableViewportUpdates();
	});
}

void TabInstance::prepareSwitch()
{
	m_scrollPos = _tabView->getScrollPos();
	is_current = false;
	prepareDerivedForSwitch();
}
