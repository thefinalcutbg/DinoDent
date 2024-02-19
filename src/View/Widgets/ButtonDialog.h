#pragma once

#include <QDialog>
#include "ui_ButtonDialog.h"

#include <vector>
#include <string>

class ButtonDialog : public QDialog
{
	Q_OBJECT

	int	m_result{ -1 };

	void paintEvent(QPaintEvent* e) override;

public:
	ButtonDialog(const std::vector<std::string>& buttonNames, const std::string& title, const std::string& description);
	int getResult() { return m_result; }
	~ButtonDialog();

private:
	Ui::ButtonDialogClass ui;

};
