#pragma once

#include <QWidget>
#include "ui_ReportFilesView.h"
#include "Presenter/ReportFilesPresenter.h"
#include "View/Models/ReportFilesModel.h"
#include "View/Interfaces/IReportFilesView.h"


class ReportFilesView : public QWidget, public IReportFilesView
{
	Q_OBJECT

	ReportFilesModel model;

	ReportFilesPresenter presenter;
	void paintEvent(QPaintEvent* event) override;
public:
	ReportFilesView(QWidget *parent = nullptr);
	
	~ReportFilesView();

private:
	Ui::ReportFilesViewClass ui;

	// Inherited via IReportFilesView
	virtual void setFiles(const std::vector<ReportFile>& fileRows) override;
};
