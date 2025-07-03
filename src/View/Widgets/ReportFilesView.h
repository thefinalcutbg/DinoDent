#pragma once

#include <QWidget>
#include "ui_ReportFilesView.h"
#include "Presenter/ReportFilesPresenter.h"
#include "View/TableModels/ReportFilesModel.h"
#include "View/Interfaces/IReportFilesView.h"


class ReportFilesView : public QWidget, public IReportFilesView
{
	Q_OBJECT

	ReportFilesModel model;
	bool m_first_show = true;
	ReportFilesPresenter presenter;

public:
	ReportFilesView(QWidget *parent = nullptr);
	void hasBeenShown();
	~ReportFilesView();

private:
	Ui::ReportFilesViewClass ui;

	// Inherited via IReportFilesView
	virtual void setFiles(const std::vector<ReportFile>& fileRows) override;
};
