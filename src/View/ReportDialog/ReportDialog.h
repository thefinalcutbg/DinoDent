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

	void paintEvent(QPaintEvent* event) override;

public:
	ReportDialog(ReportDialogPresenter* p, QWidget *parent = Q_NULLPTR);

	~ReportDialog();

private:
	Ui::ReportDialog ui;

	// Inherited via IReportDialog
	virtual void appendText(const std::string& text) override;
	virtual void clearText() override;
	virtual void setPercent(int percent) override;
	virtual void enableReportButtons(bool enabled) override;
};
