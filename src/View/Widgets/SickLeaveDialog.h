#pragma once

#include <QDialog>
#include "ui_SickLeaveDialog.h"
#include "Model/SickLeave.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SickLeaveDialogClass; };
QT_END_NAMESPACE

class SickLeaveDialog : public QDialog
{
	Q_OBJECT

	void paintEvent(QPaintEvent* e) override;

public:
	SickLeaveDialog(const SickLeave& notice);
	~SickLeaveDialog();

private:
	Ui::SickLeaveDialogClass *ui;
};

