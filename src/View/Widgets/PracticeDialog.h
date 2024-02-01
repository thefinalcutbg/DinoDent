#pragma once

#include <QDialog>
#include "ui_PracticeDialog.h"
#include "View/Interfaces/IPracticeDialog.h"

class PracticeDialogPresenter;

class PracticeDialog : public QDialog, public IAddPracticeDialog
{
	Q_OBJECT

    PracticeDialogPresenter& presenter;

	void paintEvent(QPaintEvent* event) override;

public:
    PracticeDialog(PracticeDialogPresenter& presenter, QWidget *parent = Q_NULLPTR);

	IPracticeSettings* practiceSettingsView() override;
	void closeDialog() override;

	~PracticeDialog();

private:
	Ui::PracticeDialog ui;


};
