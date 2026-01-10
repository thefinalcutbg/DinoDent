#pragma once

#include "ui_WelcomeWidget.h"
#include <QWidget>

class MainPresenter;

class WelcomeWidget : public QWidget
{
	Q_OBJECT

	void paintEvent(QPaintEvent* event) override;

public:
	WelcomeWidget(QWidget *parent = nullptr);
	~WelcomeWidget();

private:
	Ui::WelcomeWidgetClass ui;
};
