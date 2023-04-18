#pragma once

#include <QDialog>
#include "ui_MedicalStatusDialog.h"

#include <optional>

#include "Model/MedicalStatuses.h"

class MedicalStatusDialog : public QDialog
{
	Q_OBJECT

public:
	MedicalStatusDialog(const MedicalStatuses& s, QWidget *parent = nullptr);
	MedicalStatuses getResult();
	~MedicalStatusDialog();

private:
	Ui::MedicalStatusDialogClass ui;
};
