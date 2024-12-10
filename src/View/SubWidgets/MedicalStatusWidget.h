#pragma once

#include <QWidget>
#include "ui_MedicalStatusWidget.h"
#include <vector>
#include "Model/ICD10.h"

class MedicalStatusWidget : public QWidget
{
	Q_OBJECT

	std::vector<ICD10> m_icd_list;

public:
	MedicalStatusWidget(QWidget *parent = nullptr);
	void setMedicalStatus(const std::vector<ICD10>& s);
	std::vector<ICD10> getMedicalStatus();
	void setName(const QString& name);
	void addSpecialButton(QPushButton* b);
	~MedicalStatusWidget();

private:
	Ui::MedicalStatusWidgetClass ui;
};
