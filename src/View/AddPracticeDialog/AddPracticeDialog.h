#pragma once

#include <QDialog>
#include "ui_AddPracticeDialog.h"
#include "IAddPracticeDialog.h"

class AddPracticePresenter;

class AddPracticeDialog : public QDialog, public IAddPracticeDialog
{
	Q_OBJECT

	AddPracticePresenter* presenter;

	void paintEvent(QPaintEvent* event) override;

public:
	AddPracticeDialog(AddPracticePresenter* presenter, QWidget *parent = Q_NULLPTR);

	IPracticeSettings* practiceSettingsView() override;
	IPracticeDoctorSettings* doctorSettingsView() override;
	void closeDialog() override;

	~AddPracticeDialog();

private:
	Ui::AddPracticeDialog ui;


};
