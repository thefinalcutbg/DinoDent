#pragma once
#include "ITabView.h"
#include <QWidget>
#include "ui_TabView.h"
#include "Presenter/TabPresenter/TabPresenter.h"
#include "View/ListView/ListView.h"
#include "View/PerioView/PerioView.h"

class TabView : public QWidget, public ITabView
{
	Q_OBJECT

	TabPresenter* tabPresenter;
	
	PerioView m_perioView;
	ListView m_listView;

	QLabel* noTabs;

	void showTabWidget(QWidget* w);

public:
	TabView(QWidget *parent = Q_NULLPTR);
	~TabView();

	// Inherited via ITabView
	void newTab(int vecPos, std::string tabName) override;
	void focusTab(int vecPos) override;
	void removeCurrentTab() override;
	void changeTabName(std::string tabName) override;
	void setTabPresenter(TabPresenter* presenter) override;
	void removeTab(int vecPos) override;

	ScrollPos getScrollPos();
	void setScrollPos(ScrollPos scrollPos);

	void showListView();
    void showPerioView();
	void showDinosaur();

	IListView* listView() override;
	IPerioView* perioView() override;

signals:
	void closeRequested();

private:
	Ui::TabView ui;




};
