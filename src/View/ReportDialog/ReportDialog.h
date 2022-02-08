#pragma once

#include <QDialog>
#include "ui_ReportDialog.h"
#include "ReportDialogResult.h"

class ReportDialog : public QDialog
{
	Q_OBJECT

	std::optional<ReportDialogResult>& ref_result;
	void paintEvent(QPaintEvent* event) override;

public:
	ReportDialog(std::optional<ReportDialogResult>& result, QWidget *parent = Q_NULLPTR);
	~ReportDialog();

private:
	Ui::ReportDialog ui;
};
