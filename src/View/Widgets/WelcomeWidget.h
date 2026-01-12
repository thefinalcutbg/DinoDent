#pragma once

#include "ui_WelcomeWidget.h"
#include <QWidget>

class MainPresenter;

class WelcomeWidget : public QWidget
{
	Q_OBJECT

	void paintEvent(QPaintEvent* event) override;
	QStringList m_tips;
public:
	WelcomeWidget(QWidget *parent = nullptr);
	void refreshTip(bool perHour = true);
	~WelcomeWidget();

private:
	Ui::WelcomeWidgetClass ui;
};
