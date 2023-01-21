#include "ProcedureEditDialog.h"
#include "Presenter/ProcedureEditorPresenter.h"
#include "Presenter/ObturationPresenter.h"
#include "Presenter/CrownPresenter.h"



ProcedureEditDialog::ProcedureEditDialog(ProcedureEditorPresenter* p, QWidget *parent)
	: QDialog(parent), presenter(p)
{

	ui.setupUi(this);
	presenter->setView(this);

	setModal(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Редактиране на манипулация");

	connect(ui.okButton, &QPushButton::clicked, [=] {presenter->okPressed(); });
	connect(ui.cancelButton, &QPushButton::clicked, [=] { close(); });

}

ProcedureEditDialog::~ProcedureEditDialog()
{
}



ICommonFields* ProcedureEditDialog::commonFields()
{
	return ui.commonWidget;
}


void ProcedureEditDialog::closeDialog()
{
	close();
}

#include <QPainter>
void ProcedureEditDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	painter.end();
}
