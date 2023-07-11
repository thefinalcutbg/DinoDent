#pragma once

#include <QDialog>
#include "ui_MultilineDialog.h"
#include <string>

class MultilineDialog : public QDialog
{
	Q_OBJECT

public:
	MultilineDialog(const std::string& text, QWidget *parent = Q_NULLPTR);
	~MultilineDialog();

private:
	Ui::MultilineDialog ui;
};
