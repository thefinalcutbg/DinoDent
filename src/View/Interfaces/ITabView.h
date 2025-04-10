#pragma once
#include <string>

class IListView;
class IPerioView;
class IPatientSummaryView;
class IFinancialView;
class IPrescriptionView;
class CalendarView;
struct TabName;
struct ScrollPos { int height{ 0 }, width{ 0 }; };

class ITabView
{
public:
	virtual void newTab(int vecPos, const TabName& tabName) = 0;
	virtual void focusTab(int vecPos) = 0;
	virtual void removeCurrentTab() = 0;
	virtual void removeTab(int vecPos) = 0;
//	virtual void changeTabName(const TabName& tabName) = 0;
	virtual void changeTabName(const TabName& tabName, int presenterIdx) = 0;

	virtual void removeAllTabs() = 0;

	virtual ScrollPos getScrollPos() = 0;
	virtual void setScrollPos(ScrollPos scrollPos) = 0;

	virtual void showListView() = 0;
	virtual void showPerioView() = 0;
	virtual void showSummaryView() = 0;
	virtual void showFinancialView() = 0;
	virtual void showPerscriptionView() = 0;
	virtual void showWelcomeScreen() = 0;
	virtual void showCalendarView() = 0;

	virtual IListView* listView() = 0;
	virtual IPerioView* perioView() = 0;
	virtual IPrescriptionView* prescriptionView() = 0;
	virtual IPatientSummaryView* summaryView() = 0;
	virtual IFinancialView* financialView() = 0;
	virtual CalendarView* calendarView() = 0;
	
};