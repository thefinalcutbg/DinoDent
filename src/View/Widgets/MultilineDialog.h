#pragma once

#include <QDialog>
#include "ui_MultilineDialog.h"
#include <string>

class MultilineDialog : public QDialog
{
	Q_OBJECT

	std::optional<std::string> m_result;

public:
	MultilineDialog(const std::string& text, QWidget *parent = Q_NULLPTR);
	std::optional<std::string> getResult();
	void enableEditing();
	~MultilineDialog();

private:
	Ui::MultilineDialog ui;
};
