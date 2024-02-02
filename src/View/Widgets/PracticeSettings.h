#pragma once

#include <QWidget>
#include "ui_PracticeSettings.h"
#include "View/Interfaces/IPacticeSettings.h"
#include "Presenter/PracticeSettingsPresenter.h"
#include "Model/Validators/PatientValidators.h"

class PracticeSettings : public QWidget, public IPracticeSettings
{
	Q_OBJECT

	PracticeSettingsPresenter* presenter{nullptr};

	static constexpr int lineEditCount = 11;
	std::array<AbstractLineEdit*, lineEditCount> lineEdits;

	EgnLnchValidator id_validator;

	void paintEvent(QPaintEvent* event) override;

public:
	PracticeSettings(QWidget *parent = Q_NULLPTR);
	~PracticeSettings();

	void setPractice(const Practice& practice) override;
	Practice getPractice() override;
	AbstractLineEdit* lineEdit(PracticeTextFields::Field field) override;
	AbstractDateEdit* dateEdit() override;
	int legalForm() override;
	void setPresenter(PracticeSettingsPresenter* presenter) override;
	void hidePassword() override;
	void setDoctorList(const std::vector<PracticeDoctor>& doctors) override;
	void setDoctorProperties(bool admin, NhifSpecialty specialty) override;
	void replaceCurrentItem(const PracticeDoctor& item) override;


private:
	Ui::PracticeSettings ui;


};
