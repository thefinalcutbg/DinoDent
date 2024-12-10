#pragma once

#include <QDialog>
#include "ui_MedicalStatusDialog.h"

#include <optional>

#include "Model/MedicalStatuses.h"
#include "Model/Allergy.h"

class MedicalStatusPresenter;

class MedicalStatusDialog : public QDialog
{
	Q_OBJECT

	MedicalStatusPresenter* presenter;

	int getAllergyIndex();

public:
	MedicalStatusDialog(MedicalStatusPresenter* p);

	void setPastDiseases(const std::vector<ICD10>& pd);
	std::vector<ICD10> getPastDiseases();

	void setCurrentDiseases(const std::vector<ICD10>& cd);
	std::vector<ICD10> getCurrentDiseases();

	void setAllergies(const std::vector<Allergy>& allergies);

	~MedicalStatusDialog();

private:


	void paintEvent(QPaintEvent* event) override;

	Ui::MedicalStatusDialogClass ui;
};
