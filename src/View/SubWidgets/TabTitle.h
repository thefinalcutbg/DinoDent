#pragma once

#include <QWidget>
#include "ui_TabTitle.h"
#include "Presenter/TabInstance.h"

class TabView;

class TabTitle : public QWidget
{
	Q_OBJECT

	int m_id;

public:
	TabTitle(TabView* tabView, int tabId);
	const int tabId;
	void setData(const TabName& name);
	void setCurrentAppearence(bool current);
	void mouseReleaseEvent(QMouseEvent* event) override;
	int getTabId() const { return m_id; };
	~TabTitle();

private:
	Ui::TabTitle ui;
};
