#pragma once

class ProcedureDialogPresenter;
class PatientDialogPresenter;
class ProcedureEditorPresenter;
class AllergiesDialogPresenter;

namespace ModalDialogBuilder
{

	void openDialog(ProcedureDialogPresenter* p);
	void openDialog(PatientDialogPresenter* p);
	void openDialog(ProcedureEditorPresenter* p);
	void openDialog(AllergiesDialogPresenter* p);
};

