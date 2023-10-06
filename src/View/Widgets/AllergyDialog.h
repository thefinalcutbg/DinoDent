#pragma once

#include <QDialog>
#include "ui_AllergyDialog.h"

class AllergyDialog : public QDialog
{
	Q_OBJECT


public:
	AllergyDialog(QWidget *parent = nullptr);
	~AllergyDialog();

private:
	Ui::AllergyDialog ui;
};
