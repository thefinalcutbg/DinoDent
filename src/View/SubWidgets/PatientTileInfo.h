#pragma once

#include <QWidget>

#include "ui_PatientTileInfo.h"
#include "View/uiComponents/RoundedFrame.h"

struct PatientInfoPresenter;

struct PatientTileInfo : public RoundedFrame
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
