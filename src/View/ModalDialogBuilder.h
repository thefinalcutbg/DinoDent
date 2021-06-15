#pragma once

class ProcedureDialogPresenter;
class PatientDialogPresenter;
class ProcedureEditorPresenter;

class ModalDialogBuilder
{

	ModalDialogBuilder();

public:
	ModalDialogBuilder(const ModalDialogBuilder&) = delete;

	static void openDialog(ProcedureDialogPresenter* p);
	static void openDialog(PatientDialogPresenter* p);
	static void openDialog(ProcedureEditorPresenter* p);
};

