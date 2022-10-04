#pragma once
#include "View/Interfaces/IReportFilesView.h"
#include "Network/PIS/ReportFilesService.h"

class ReportFilesPresenter
{
	ReportFilesService service;
	std::vector<ReportFile> reportFiles;
	IReportFilesView* view{ nullptr };

public:
	void setView(IReportFilesView* view);
	void refreshFiles();
	void showError(int idx);

};