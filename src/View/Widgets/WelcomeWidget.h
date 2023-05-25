#pragma once

#include <QWidget>
#include "ui_WelcomeWidget.h"

class WelcomeWidget : public QWidget
{
	Q_OBJECT

public:
	WelcomeWidget(QWidget *parent = nullptr);
	~WelcomeWidget();

private:
	Ui::WelcomeWidgetClass ui;
};
