#pragma once

#include <QWidget>
#include "ui_PracticeDoctorSettings.h"
#include "View/Interfaces/IPracticeDoctorSettings.h"

class PracticeDoctorSettings : public QWidget, public IPracticeDoctorSettings
{
	Q_OBJECT

	PracticeDoctorSettingsPresenter* presenter{nullptr};

	void paintEvent(QPaintEvent* event) override;

public:
	PracticeDoctorSettings(QWidget *parent = Q_NULLPTR);
	void setDoctorList(const std::vector<PracticeDoctor>& doctors) override;
	void setPresenter(PracticeDoctorSettingsPresenter* presenter) override;
	void setDoctorProperties(bool admin, NhifSpecialty spec) override;
	void hideDoctorProperties() override;
	void replaceCurrentItem(const PracticeDoctor& item) override;
	~PracticeDoctorSettings();

private:
	Ui::PracticeDoctorSettings ui;
};
