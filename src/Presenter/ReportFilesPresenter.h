#pragma once

#include "Network/PIS/ReportFilesService.h"

class ReportFilesView;

class ReportFilesPresenter
{
	ReportFilesService service;
	static inline std::vector<ReportFile> reportFiles;
	ReportFilesView* view{ nullptr };

public:
	void setView(ReportFilesView* view);
	void refreshFiles();
	void showError(int idx);

	static void clearCache() {
		reportFiles.clear();
	}

};