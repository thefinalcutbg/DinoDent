#pragma once

#include <QWidget>
#include "ui_TabTitle.h"

class TabBar;

class TabTitle : public QWidget
{
	Q_OBJECT

	TabBar* m_parent;

public:
	TabTitle(TabBar* parent, int tabId);
	const int tabId;
	void setText(const QString& header, const QString& footer);
	void setCurrentAppearence(bool current);
	~TabTitle();

private:
	Ui::TabTitle ui;
};
