#pragma once

#include <QWidget>

#include "View/Widgets/ListView.h"
#include "View/Widgets/PerioView.h"
#include "View/Widgets/FinancialView.h"
#include "View/Widgets/PrescriptionView.h"
#include "View/uiComponents/PixmapLabel.h"
#include "View/Widgets/WelcomeWidget.h"
#include "View/Widgets/CalendarView.h"
#include "ui_TabView.h"

class TabTitle;

class TabView : public QWidget
{
	Q_OBJECT

	PerioView m_perioView;
	ListView m_listView;
	FinancialView m_financialView;
	PrescriptionView m_prescriptionView;
	CalendarView m_calendarView;

	WelcomeWidget welcomeScreen{ nullptr };

	void showTabWidget(QWidget* w);

	TabTitle* getTabTitle(int tabId);
	int getTabIndex(int tabId);


public:

	TabView(QWidget *parent = Q_NULLPTR);
	~TabView();

	void requestClose(int tabId);

	void removeAllTabs();
	void newTab(int tabId, const TabName& tabName);
	void focusTab(int tabId);
	void removeCurrentTab();
	//void changeTabName(const TabName& tabName) override;
	void changeTabName(const TabName& tabName, int tabId);

	void removeTab(int tabId);

	std::pair<int, int> getScrollPos();
	void setScrollPos(std::pair<int, int> scrollPos);

	void showListView();
    void showPerioView();
	void showFinancialView();
	void showPerscriptionView();
	void showWelcomeScreen();
	void showCalendarView();

	ListView* listView() { return &m_listView; }
	PerioView* perioView() { return &m_perioView; }
	FinancialView* financialView() { return &m_financialView; }
	PrescriptionView* prescriptionView() { return &m_prescriptionView; }
	CalendarView* calendarView() { return &m_calendarView; }

signals:
	void closeRequested(int mapIndex);

private:
	Ui::TabView ui;




};
