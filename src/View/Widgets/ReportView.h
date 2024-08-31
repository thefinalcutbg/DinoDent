#pragma once

#include <QWidget>
#include "ui_ReportView.h"
#include "ReportDialogResult.h"
#include "View/Interfaces/IReportView.h"
#include "Presenter/ReportPresenter.h"

class ReportView : public QWidget, public IReportView
{
	Q_OBJECT

	ReportPresenter presenter;
	//if true, generate button acts as abort button
	bool m_stop{false};

public:
	ReportView(QWidget *parent = Q_NULLPTR);

	~ReportView();

private:
	Ui::ReportViewClass ui;

	// Inherited via IReportDialog
	void appendSheet(const std::string& numberLink, const std::string& description);
	void appendText(const std::string& text) override;
	void clearText() override;
	void setPercent(int percent) override;
	void enableReportButtons(bool enabled) override;
	void showStopButton(bool yes) override;
	void closeDialog() override;
};
