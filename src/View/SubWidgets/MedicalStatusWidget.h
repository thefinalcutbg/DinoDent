#pragma once

#include <QWidget>
#include "ui_MedicalStatusWidget.h"
#include <vector>
#include "Model/MedicalStatuses.h"


class MedicalStatusWidget : public QWidget
{
	Q_OBJECT

public:
	MedicalStatusWidget(QWidget *parent = nullptr);
	void setMedicalStatus(const std::vector<MedicalStatus>& s);
	std::vector<MedicalStatus> getMedicalStatus();
	void setName(const QString& name);
	~MedicalStatusWidget();

private:
	Ui::MedicalStatusWidgetClass ui;
};
