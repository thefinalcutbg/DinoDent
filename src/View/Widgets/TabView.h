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
class SmoothWheelScroll;

class TabView : public QWidget
{
	Q_OBJECT

	WelcomeWidget* welcomeScreen{ nullptr };
	PerioView* m_perioView{ nullptr };
	ListView* m_listView{ nullptr };
	FinancialView* m_financialView{ nullptr };
	PrescriptionView* m_prescriptionView{ nullptr };
	TreatmentPlanView* m_treatmentView{ nullptr };
	CalendarView* m_calendarView{ nullptr };

    bool showFocusedTabBorder = true;

	TabTitle* getTabTitle(int tabId);
	int getTabIndex(int tabId);

	void refreshTabBorder(QScrollArea* sa);

	void initTabs();

public:

	TabView(QWidget* parent = Q_NULLPTR);
	~TabView();

	void enableViewportUpdates(TabType t);
	void disableViewportUpdates(TabType t);

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

	void showView(TabType t);

	void showListView();
	void showPerioView();
	void showFinancialView();
	void showPerscriptionView();
	void showWelcomeScreen();
	void showTreatmentPlanView();
	void showCalendarView();

	ListView* listView() { return m_listView; }
	PerioView* perioView() { return m_perioView; }
	FinancialView* financialView() { return m_financialView; }
	PrescriptionView* prescriptionView() { return m_prescriptionView; }
	TreatmentPlanView* treatmentPlanView() { return m_treatmentView; }
	CalendarView* calendarView() { return m_calendarView; }
	QScrollArea* scrollArea();
signals:
	void closeRequested(int mapIndex);

private:
	Ui::TabView ui;




};