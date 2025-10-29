#pragma once

#include <QWidget>
#include "ui_ReportView.h"
#include "ReportDialogResult.h"
#include "Presenter/ReportPresenter.h"

class ReportView : public QWidget
{
	Q_OBJECT

	ReportPresenter presenter;
	//if true, generate button acts as abort button
	bool m_stop{false};

public:
	ReportView(QWidget *parent = Q_NULLPTR);

	void appendSheet(long long sheetRowid, const std::string& sheetNumber, const std::string& description);
	void appendText(const std::string& text);
	void clearText();
	void setPercent(int percent);
	void enableReportButtons(bool enabled);
	void showStopButton(bool yes);
	void closeDialog();

	~ReportView();

private:
	Ui::ReportViewClass ui;


};
