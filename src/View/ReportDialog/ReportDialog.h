#pragma once

#include <QDialog>
#include "ui_ReportDialog.h"
#include "ReportDialogResult.h"
#include "IReportDialog.h"

class ReportDialogPresenter;

class ReportDialog : public QDialog, public IReportDialog
{
	Q_OBJECT

	ReportDialogPresenter* presenter;
	//if true, generate button acts as abort button
	bool m_stop{false};

	void paintEvent(QPaintEvent* event) override;

public:
	ReportDialog(ReportDialogPresenter* p, QWidget *parent = Q_NULLPTR);

	~ReportDialog();

private:
	Ui::ReportDialog ui;

	// Inherited via IReportDialog
	void appendText(const std::string& text) override;
	void clearText() override;
	void setPercent(int percent) override;
	void enableReportButtons(bool enabled) override;
	void showStopButton(bool yes) override;
};
