#pragma once

#include <QWidget>

#include "View/Interfaces/IPatientTileInfo.h"

#include "ui_PatientTileInfo.h"

class PatientInfoPresenter;

class PatientTileInfo : public QWidget, public IPatientTileInfo
{
	Q_OBJECT

	PatientInfoPresenter* presenter{ nullptr };

public:
	PatientTileInfo(QWidget *parent = nullptr);
	void setPatient(const Patient& p, int age) override;
	void setPresenter(PatientInfoPresenter* p) override { presenter = p; }

	~PatientTileInfo();

private:
	Ui::PatientTileInfoClass ui;
};
