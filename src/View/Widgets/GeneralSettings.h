#pragma once

#include <QWidget>
#include "ui_GeneralSettings.h"

class GeneralSettings : public QWidget
{
	Q_OBJECT

	void paintEvent(QPaintEvent* event) override;

public:
	GeneralSettings(QWidget *parent = nullptr);
	
	~GeneralSettings();

	Ui::GeneralSettingsClass ui;
};
