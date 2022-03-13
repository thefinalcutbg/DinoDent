#pragma once

#include <QTabBar>
#include <QVariant>
#include <QPushButton>

class TabBar : public QTabBar
{
	Q_OBJECT
	int m_currentIdx{ -1 };
	void mouseReleaseEvent(QMouseEvent* event) override;
	using QTabBar::removeTab;
	void changeActiveTabAppearence(int idx);

public:
	TabBar(QWidget *parent);

	void requestClose(QWidget* titleWidget);
	void addNewTab(int mapIndex, const QString& header, const QString& footer);
	void changeTabName(int tabIndex, const QString& header, const QString& footer);
	int getTabId(int tabIndex);
	void closeTab(int index);

	~TabBar();
};
