#pragma once

#include <QDialog>

#include "Model/Validators/PrescriptionValidators.h"

#include "ui_MedicationDialog.h"

struct Medication;
class MedicationPresenter;

class MedicationDialog : public QDialog
{
	Q_OBJECT

    MedicationPresenter& presenter;
	MedicationNameValidator nameValidator;

	void commonDataChanged();
    void periodChanged();

public:
    MedicationDialog(MedicationPresenter& p, QWidget *parent = nullptr);
	void setQuantityListNames(const std::string& pack, const std::string& form);
	void setFormLabel(const std::string& formName);
	void setDosageList(const std::vector<std::string> dosageList);
	void setReadOnly();
	bool fieldsAreValid();

	void setMedication(const Medication& m);

	~MedicationDialog();

private:
	Ui::MedicationDialogClass ui;

};
