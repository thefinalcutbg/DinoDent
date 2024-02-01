#pragma once

#include <QWidget>
#include "ui_MedicalStatusWidget.h"
#include <vector>

class MedicalStatusWidget : public QWidget
{
	Q_OBJECT

public:
	MedicalStatusWidget(QWidget *parent = nullptr);
	void setMedicalStatus(const std::vector<std::string>& s);
	std::vector<std::string> getMedicalStatus();
	void setName(const QString& name);
	void addSpecialButton(QPushButton* b);
	~MedicalStatusWidget();

private:
	Ui::MedicalStatusWidgetClass ui;
};
