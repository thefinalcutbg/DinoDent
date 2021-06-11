#include "ModalDialogBuilder.h"
#include "View/ProcedureDialog/ProcedureDialog.h"

ModalDialogBuilder ModalDialogBuilder::_instance;

ModalDialogBuilder::ModalDialogBuilder()
{
}

void ModalDialogBuilder::openDialog(ProcedureDialogPresenter* p)
{
	ProcedureDialog d(p);
	d.exec();
}

ModalDialogBuilder& ModalDialogBuilder::instance()
{
	return _instance;
}
