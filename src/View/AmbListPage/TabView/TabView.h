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
	void newTab(int vecPos, std::string tabName) override;
	void focusTab(int vecPos) override;
	void removeCurrentTab() override;
	void changeTabName(std::string tabName) override;
	void setTabPresenter(TabPresenter* presenter) override;
	void removeTab(int vecPos) override;

	ScrollPos getScrollPos();
	void setScrollPos(ScrollPos scrollPos);

	virtual IListView* listView() override;

signals:
	void closeRequested();

private:
	Ui::TabView ui;




};
