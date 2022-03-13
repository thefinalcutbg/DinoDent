#pragma once
#include "ITabView.h"
#include <QWidget>
#include "ui_TabView.h"
#include "Presenter/TabPresenter/TabPresenter.h"
#include "View/ListView/ListView.h"
#include "View/PerioView/PerioView.h"
#include "View/PatientSummaryView/PatientSummary.h"

class TabView : public QWidget, public ITabView
{
	Q_OBJECT

	TabPresenter* tabPresenter;
	
	PerioView m_perioView;
	ListView m_listView;
	PatientSummary m_summaryView;

	QLabel* noTabs;

	void showTabWidget(QWidget* w);

public:
	TabView(QWidget *parent = Q_NULLPTR);
	~TabView();

	// Inherited via ITabView
	void newTab(int vecPos, const TabName& tabName) override;
	void focusTab(int vecPos) override;
	void removeCurrentTab() override;
	void changeTabName(const TabName& tabName) override;
	void setTabPresenter(TabPresenter* presenter) override;
	void removeTab(int vecPos) override;

	ScrollPos getScrollPos();
	void setScrollPos(ScrollPos scrollPos);

	void showListView() override;
    void showPerioView() override;
	void showSummaryView() override;
	void showDinosaur() override;

	IListView* listView() override;
	IPerioView* perioView() override;
	IPatientSummaryView* summaryView() override;

signals:
	void closeRequested(int mapIndex);

private:
	Ui::TabView ui;




};
