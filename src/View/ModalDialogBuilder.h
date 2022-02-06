#pragma once
#include <map>
#include <string>
#include "View/ReportDialog/ReportDialogResult.h"

class ProcedureDialogPresenter;
class PatientDialogPresenter;
class ProcedureEditorPresenter;
class AllergiesDialogPresenter;
class ListSelectorPresenter;
class DetailedStatusPresenter;
class LoginPresenter;

enum class DialogAnswer { Yes, No, Cancel };

namespace ModalDialogBuilder
{
	void openDialog(ProcedureDialogPresenter* p);
	void openDialog(PatientDialogPresenter* p);
	void openDialog(ProcedureEditorPresenter* p);
	void openDialog(AllergiesDialogPresenter* p);
	void openDialog(ListSelectorPresenter* p);
	void openDialog(DetailedStatusPresenter* p);
	void openDialog(LoginPresenter* p);
	void openDialog(std::optional<ReportDialogResult>& result);
	int openSaveAsDialog(int newNum, std::map<int, bool> existingNumbers);
	DialogAnswer openSaveDialog(const std::string& text);
	bool askDialog(const std::string& questionText);
	void showError(const std::string& error);
};

