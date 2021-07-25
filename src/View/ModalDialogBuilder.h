#pragma once

class ProcedureDialogPresenter;
class PatientDialogPresenter;
class ProcedureEditorPresenter;
class AllergiesDialogPresenter;
class ListSelectorPresenter;
class DetailsPresenter;

namespace ModalDialogBuilder
{
	void openDialog(ProcedureDialogPresenter* p);
	void openDialog(PatientDialogPresenter* p);
	void openDialog(ProcedureEditorPresenter* p);
	void openDialog(AllergiesDialogPresenter* p);
	void openDialog(ListSelectorPresenter* p);
	void openDialog(DetailsPresenter* p);
};

