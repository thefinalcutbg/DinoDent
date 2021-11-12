#pragma once

#include <QTabBar>
#include <QVariant>
#include <QPushButton>

class TabBar : public QTabBar
{
	Q_OBJECT


	void mouseReleaseEvent(QMouseEvent* event) override;
	using QTabBar::removeTab;

public:
	TabBar(QWidget *parent);

	void addNewTab(QString tabName, QVariant vecPos);
	void closeTab(int index);

	~TabBar();
};
