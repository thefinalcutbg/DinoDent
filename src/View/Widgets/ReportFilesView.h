#pragma once

#include <QWidget>
#include "ui_ReportFilesView.h"
#include "Presenter/ReportFilesPresenter.h"
#include "View/TableModels/ReportFilesModel.h"


class ReportFilesView : public QWidget
{
	Q_OBJECT

	ReportFilesModel model;
	bool m_first_show = true;
	ReportFilesPresenter presenter;

public:
	ReportFilesView(QWidget *parent = nullptr);
	void hasBeenShown();
	void setFiles(const std::vector<ReportFile>& fileRows);
	~ReportFilesView();

private:
	Ui::ReportFilesViewClass ui;


};
