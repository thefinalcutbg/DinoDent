#include "ModalDialogBuilder.h"
#include "View/ProcedureDialog/ProcedureDialog.h"
#include "View/PatientDialog/PatientFormDialog.h"
#include "View/ProcedureEditDialog/ProcedureEditDialog.h"

ModalDialogBuilder::ModalDialogBuilder()
{
}

void ModalDialogBuilder::openDialog(ProcedureDialogPresenter* p)
{
	ProcedureDialog d(p);
	d.exec();
}

void ModalDialogBuilder::openDialog(PatientDialogPresenter* p)
{
	PatientFormDialog d(p);
	d.exec();
}

void ModalDialogBuilder::openDialog(ProcedureEditorPresenter* p)
{
	ProcedureEditDialog d(p);
	d.exec();
}
