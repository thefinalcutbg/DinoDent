#pragma once
#include "ITabView.h"
#include <QWidget>
#include "ui_TabView.h"
#include "Presenter/TabPresenter/TabPresenter.h"
#include "../../ListView/ListView.h"

class TabView : public QWidget, public ITabView
{
	Q_OBJECT

	TabPresenter* tabPresenter;

	ListView _listView;

	QLabel* noTabs;

public:
	TabView(QWidget *parent = Q_NULLPTR);
	~TabView();

	// Inherited via ITabView
	virtual void newTab(int vecPos, std::string tabName) override;
	virtual void focusTab(int vecPos) override;
	virtual void removeCurrentTab() override;
	virtual void changeTabName(std::string tabName) override;
	virtual void setTabPresenter(TabPresenter* presenter) override;

	ScrollPos getScrollPos();
	void setScrollPos(ScrollPos scrollPos);

	virtual IListView* listView() override;

signals:
	void closeRequested();

private:
	Ui::TabView ui;




};
