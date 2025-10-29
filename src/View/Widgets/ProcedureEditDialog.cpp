#include "ProcedureEditDialog.h"
#include "Presenter/ProcedureEditorPresenter.h"
#include <QPainter>

ProcedureEditDialog::ProcedureEditDialog(ProcedureEditorPresenter& p, QWidget *parent)
	: QDialog(parent), presenter(p)
{
	ui.setupUi(this);
    presenter.setView(this);

	setModal(true);
	setWindowTitle("Редактиране на процедура");

    connect(ui.okButton, &QPushButton::clicked, [&] {presenter.okPressed(); });
    connect(ui.cancelButton, &QPushButton::clicked, [&] { close(); });
}

ProcedureEditDialog::~ProcedureEditDialog() {}



ProcedureInput* ProcedureEditDialog::procedureInput()
{
	return ui.procedureInput;
}

void ProcedureEditDialog::paintEvent(QPaintEvent*)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	painter.end();
}
