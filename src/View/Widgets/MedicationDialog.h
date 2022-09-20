#pragma once

#include <QDialog>

#include "View/Interfaces/IMedicationDialog.h"
#include "Presenter/MedicationPresenter.h"
#include "Model/Validators/PrescriptionValidators.h"

#include "ui_MedicationDialog.h"

class MedicationDialog : public QDialog, public IMedicationDialog
{
	Q_OBJECT

	MedicationPresenter* presenter;
	MedicationNameValidator nameValidator;

	void commonDataChanged();

public:
	MedicationDialog(MedicationPresenter* p, QWidget *parent = nullptr);
	void setQuantityListNames(const std::string& pack, const std::string& form) override;
	void setFormLabel(const std::string& formName) override;
	void setDosageList(const std::vector<std::string> dosageList) override;

	bool fieldsAreValid() override;
	void closeUi() override { close(); }

	void setMedication(const Medication& m) override;

	~MedicationDialog();

private:
	Ui::MedicationDialogClass ui;

};
