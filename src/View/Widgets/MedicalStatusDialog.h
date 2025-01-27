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

	void setPastDiseases(const std::vector<MedicalStatus>& pd);
	std::vector<MedicalStatus> getPastDiseases();

	void setCurrentDiseases(const std::vector<MedicalStatus>& cd);
	std::vector<MedicalStatus> getCurrentDiseases();

	void setAllergies(const std::vector<Allergy>& allergies);

	~MedicalStatusDialog();

private:


	void paintEvent(QPaintEvent* event) override;

	Ui::MedicalStatusDialogClass ui;
};
