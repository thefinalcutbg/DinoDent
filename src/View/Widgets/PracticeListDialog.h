#pragma once

#include <QDialog>
#include "ui_PracticeListDialog.h"
#include <vector>
#include <string>

class PracticeListDialog : public QDialog
{
	Q_OBJECT

	void reject() override { done(-1); }
	void paintEvent(QPaintEvent* event) override;

public:
	PracticeListDialog(QDialog *parent = Q_NULLPTR);
	int exec(const std::vector<std::string>& practiceList);
	~PracticeListDialog();

private:
	Ui::PracticeListDialog ui;
};
