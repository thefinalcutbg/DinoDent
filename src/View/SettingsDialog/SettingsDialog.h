#pragma once

#include <QDialog>
#include "ui_SettingsDialog.h"

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	SettingsDialog(QDialog *parent = Q_NULLPTR);
	~SettingsDialog();

private:
	Ui::SettingsDialog ui;
};
