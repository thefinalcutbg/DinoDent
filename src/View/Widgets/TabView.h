#pragma once

#include <QWidget>

#include "Presenter/TabPresenter.h"
#include "View/Widgets/ListView.h"
#include "View/Widgets/PerioView.h"
#include "View/Widgets/PatientSummary.h"
#include "View/Widgets/FinancialView.h"
#include "View/uiComponents/PixmapLabel.h"

#include "View/Interfaces/ITabView.h"
#include "ui_TabView.h"

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
	void removeAllTabs();
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
