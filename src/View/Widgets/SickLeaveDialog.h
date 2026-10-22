#pragma once

#include <QDialog>
#include "ui_SickLeaveDialog.h"
#include "Model/SickLeave.h"
#include "Model/Validators/DateValidator.h"
#include "Model/Validators/PatientValidators.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SickLeaveDialogClass; };
QT_END_NAMESPACE

class SickLeaveDialog : public QDialog
{
	Q_OBJECT

	std::optional<SickLeave> m_result;

	NotEmptyValidator not_empty_validator;
	CityValidator city_validator;
	ICDValidator icd_validator{ false };
	void caregiverLogic();
	void paintEvent(QPaintEvent* e) override;
	bool isValid();
	void addCaregiver();

	long long m_caregiver_rowid = 0;

public:
	SickLeaveDialog(const SickLeave& notice);
	std::optional<SickLeave> getResult();
	~SickLeaveDialog();

private:
	Ui::SickLeaveDialogClass *ui;
};

