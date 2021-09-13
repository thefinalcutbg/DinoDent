#pragma once
#include <map>
#include <string>

class ProcedureDialogPresenter;
class PatientDialogPresenter;
class ProcedureEditorPresenter;
class AllergiesDialogPresenter;
class ListSelectorPresenter;
class DetailedStatusPresenter;

enum class DialogAnswer { Yes, No, Cancel };

namespace ModalDialogBuilder
{
	void openDialog(ProcedureDialogPresenter* p);
	void openDialog(PatientDialogPresenter* p);
	void openDialog(ProcedureEditorPresenter* p);
	void openDialog(AllergiesDialogPresenter* p);
	void openDialog(ListSelectorPresenter* p);
	void openDialog(DetailedStatusPresenter* p);
	int openSaveAsDialog(int newNum, std::map<int, bool> existingNumbers);
	DialogAnswer openSaveDialog(const std::string& text);
};

