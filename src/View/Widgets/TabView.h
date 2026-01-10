#pragma once

#include <QWidget>

#include "View/Widgets/ListView.h"
#include "View/Widgets/PerioView.h"
#include "View/Widgets/FinancialView.h"
#include "View/Widgets/PrescriptionView.h"
#include "View/uiComponents/PixmapLabel.h"
#include "View/Widgets/WelcomeWidget.h"
#include "View/Widgets/CalendarView.h"
#include "View/Widgets/TreatmentPlanView.h"
#include "ui_TabView.h"

class TabTitle;

class TabView : public QWidget
{
	Q_OBJECT

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
    void showTreatmentPlanView();
	void showCalendarView();

	ListView* listView() { return ui.listWidget; }
    PerioView* perioView() { return ui.perioWidget; }
	FinancialView* financialView() { return ui.financialWidget; }
	PrescriptionView* prescriptionView() { return ui.prescrWidget; }
    TreatmentPlanView* treatmentPlanView() { return ui.planView; }
	CalendarView* calendarView() { return ui.calendarPage; }

signals:
	void closeRequested(int mapIndex);

private:
	Ui::TabView ui;




};
