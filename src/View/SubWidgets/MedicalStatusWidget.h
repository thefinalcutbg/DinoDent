#pragma once

#include <QWidget>
#include "ui_MedicalStatusWidget.h"
#include <vector>
#include "Model/MedicalStatuses.h"

class MedicalStatusWidget : public QWidget
{
	Q_OBJECT

	std::vector<MedicalStatus> m_status_list;

public:
	MedicalStatusWidget(QWidget *parent = nullptr);
	void setMedicalStatus(const std::vector<MedicalStatus>& s);
	std::vector<MedicalStatus> getMedicalStatus();
	void setName(const QString& name);
	void addSpecialButton(QPushButton* b);
	~MedicalStatusWidget();

private:
	Ui::MedicalStatusWidgetClass ui;
};
