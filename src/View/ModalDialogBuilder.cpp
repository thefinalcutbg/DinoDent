#include "ModalDialogBuilder.h"

#include "View/ProcedureDialog/ProcedureDialog.h"

void ModalDialogBuilder::openDialog(ProcedureDialogPresenter* p)
{
	ProcedureDialog d(p);
	d.exec();
}

#include "View/PatientDialog/PatientFormDialog.h"

void ModalDialogBuilder::openDialog(PatientDialogPresenter* p)
{
	PatientFormDialog d(p);
	d.exec();
}

#include "View/ProcedureEditDialog/ProcedureEditDialog.h"

void ModalDialogBuilder::openDialog(ProcedureEditorPresenter* p)
{
	ProcedureEditDialog d(p);
	d.exec();
}

#include "View/AllergiesDialog/AllergiesDialog.h"

void ModalDialogBuilder::openDialog(AllergiesDialogPresenter* p)
{
	AllergiesDialog d(p);
	d.exec();
}
