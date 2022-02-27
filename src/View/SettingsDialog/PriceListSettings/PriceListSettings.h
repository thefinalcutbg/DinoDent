#pragma once

#include <QWidget>
#include "ui_PriceListSettings.h"

class PriceListSettings : public QWidget
{
	Q_OBJECT

public:
	PriceListSettings(QWidget *parent = Q_NULLPTR);
	~PriceListSettings();

private:
	Ui::PriceListSettings ui;
};
