#pragma once

#include <QWidget>

#include "ui_PatientTileInfo.h"

struct PatientInfoPresenter;

struct PatientTileInfo : public QWidget
{
	Q_OBJECT

	PatientInfoPresenter* presenter{ nullptr };
    QMenu* context_menu;
    bool context_menu_overPatient = true;

public:
	PatientTileInfo(QWidget *parent = nullptr);
	void setPatient(const Patient& p, int age);
	void setPresenter(PatientInfoPresenter* p) { presenter = p; }

	~PatientTileInfo();

private:
	Ui::PatientTileInfoClass ui;
};
