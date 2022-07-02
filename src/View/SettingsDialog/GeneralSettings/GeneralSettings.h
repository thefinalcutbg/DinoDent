#pragma once

#include <QWidget>
#include "ui_GeneralSettings.h"

class GeneralSettings : public QWidget
{
	Q_OBJECT

public:
	GeneralSettings(QWidget *parent = nullptr);
	~GeneralSettings();

private:
	Ui::GeneralSettingsClass ui;
};
