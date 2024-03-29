#pragma once

#include <QWidget>
#include "ui_TabTitle.h"

class TabView;

class TabTitle : public QWidget
{
	Q_OBJECT

	int m_id;

public:
	TabTitle(TabView* tabView, int tabId);
	const int tabId;
	void setText(const QString& header, const QString& footer);
	void setCurrentAppearence(bool current);
	void mouseReleaseEvent(QMouseEvent* event) override;
	void setFooterIcon(const QPixmap& px);
	void setHeaderIcon(const QPixmap& px);
	int getTabId() const { return m_id; };
	~TabTitle();

private:
	Ui::TabTitle ui;
};
