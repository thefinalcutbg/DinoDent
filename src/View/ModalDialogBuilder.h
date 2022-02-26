#pragma once
#include <unordered_set>
#include <string>
#include "View/ReportDialog/ReportDialogResult.h"

class ProcedureDialogPresenter;
class PatientDialogPresenter;
class ProcedureEditorPresenter;
class AllergiesDialogPresenter;
class ListSelectorPresenter;
class DetailedStatusPresenter;
class LoginPresenter;
class DoctorDialogPresenter;
class InvoicePresenter;

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
	void openDialog(DoctorDialogPresenter* p);
	void openDialog(InvoicePresenter* p);
	int saveAsAmbSheetNumber(int newNum, std::unordered_set<int> existingNumbers);
	DialogAnswer openSaveDialog(const std::string& text);
	bool askDialog(const std::string& questionText);
	void showError(const std::string& error);
	void showErrorList(const std::string& errors);
};

