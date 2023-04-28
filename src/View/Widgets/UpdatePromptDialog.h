#pragma once

#include <string> 
#include <QDialog>

#include "ui_UpdatePromptDialog.h"

class UpdatePromptDialog : public QDialog
{
	Q_OBJECT

		void paintEvent(QPaintEvent* e) override;

public:
	UpdatePromptDialog(const std::string& changeLog, QWidget *parent = nullptr);
	~UpdatePromptDialog();

private:
	Ui::UpdatePromptDialogClass ui;
};
