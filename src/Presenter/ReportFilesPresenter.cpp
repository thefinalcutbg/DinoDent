#include "ReportFilesPresenter.h"
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"

void ReportFilesPresenter::setView(IReportFilesView* view)
{
	this->view = view;
	view->setFiles(reportFiles);
}

void ReportFilesPresenter::refreshFiles()
{
    service.sendRequest(User::practice().rziCode,

        [=, this](const std::vector<ReportFile>& files)
		{
			if (files.empty()) return;

			reportFiles = files;

			if(view)view->setFiles(files);
		}
	);
}

void ReportFilesPresenter::showError(int idx)
{
	if (idx < 0 || idx >= reportFiles.size()) return;

	if (reportFiles[idx].error.empty()) return;

    ModalDialogBuilder::showMultilineDialog(reportFiles[idx].error, "Грешка от ПИС");
}
