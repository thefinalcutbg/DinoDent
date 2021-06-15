#include "ProcedureEditDialog.h"
#include "Presenter/ProcedureDialog/ProcedureEditorPresenter.h"
#include "Presenter/ProcedureDialog/SubPresenters/ObturationPresenter.h"
#include "Presenter/ProcedureDialog/SubPresenters/CrownPresenter.h"
#include "Presenter/ProcedureDialog/SubPresenters/ImplantPresenter.h"

ProcedureEditDialog::ProcedureEditDialog(ProcedureEditorPresenter* p, QWidget *parent)
	: QDialog(parent), presenter(p)
{

	ui.setupUi(this);
	presenter->setView(this);
	setWindowTitle("Редактиране на манипулация");

	connect(ui.okButton, &QPushButton::clicked, [=] {presenter->okPressed(); });
	connect(ui.cancelButton, &QPushButton::clicked, [=] { close(); });
}

ProcedureEditDialog::~ProcedureEditDialog()
{
}

void ProcedureEditDialog::setMtype(ManipulationType m)
{
	switch (m)
	{
	case ManipulationType::obturation:
		ui.stackedWidget->setCurrentIndex(1);
		break;
	case ManipulationType::crown:
	case ManipulationType::bridge:
		ui.stackedWidget->setCurrentIndex(2);
		break;
	case ManipulationType::implant:
		ui.stackedWidget->setCurrentIndex(3);
		break;
	default:
		setMaximumSize(QSize(311, 310));
		setMinimumSize(QSize(311, 310));
		ui.okButton->move(QPoint(220, 270));
		ui.cancelButton->move(QPoint(130, 270));

	}
}

ICommonFields* ProcedureEditDialog::commonFields()
{
	return ui.commonWidget;
}

ICrownView* ProcedureEditDialog::crownView()
{
	return ui.crownWidget;
}

IObturationView* ProcedureEditDialog::obturationView()
{
	return ui.obturWidget;
}

IImplantView* ProcedureEditDialog::implantView()
{
	return ui.implantWidget;
}

void ProcedureEditDialog::closeDialog()
{
	close();
}

