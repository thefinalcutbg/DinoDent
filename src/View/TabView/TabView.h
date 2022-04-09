#pragma once
#include "ITabView.h"
#include <QWidget>
#include "ui_TabView.h"
#include "Presenter/TabPresenter/TabPresenter.h"
#include "View/ListView/ListView.h"
#include "View/PerioView/PerioView.h"
#include "View/PatientSummaryView/PatientSummary.h"
#include "View/FinancialView/FinancialView.h"
#include "View/uiComponents/qt_derived/PixmapLabel.h"
class TabTitle;

class TabView : public QWidget, public ITabView
{
	Q_OBJECT

	TabPresenter* tabPresenter;
	
	PerioView m_perioView;
	ListView m_listView;
	PatientSummary m_summaryView;
	FinancialView m_financialView;

	PixmapLabel* noTabs;

	void showTabWidget(QWidget* w);

	TabTitle* getTabTitle(int tabId);
	int getTabIndex(int tabId);
	void setTabIcon(int tabAt, bool nzok);

public:
	TabView(QWidget *parent = Q_NULLPTR);
	~TabView();

	void requestClose(int tabId);

	// Inherited via ITabView
	void newTab(int tabId, const TabName& tabName) override;
	void focusTab(int tabId) override;
	void removeCurrentTab() override;
	void changeTabName(const TabName& tabName) override;
	void changeTabName(const TabName& tabName, int tabId) override;
	void setTabPresenter(TabPresenter* presenter) override;
	void removeTab(int tabId) override;

	ScrollPos getScrollPos();
	void setScrollPos(ScrollPos scrollPos);

	void showListView() override;
    void showPerioView() override;
	void showSummaryView() override;
	void showFinancialView() override;
	void showDinosaur() override;

	IListView* listView() override;
	IPerioView* perioView() override;
	IPatientSummaryView* summaryView() override;
	IFinancialView* financialView() override;

signals:
	void closeRequested(int mapIndex);

private:
	Ui::TabView ui;




};
