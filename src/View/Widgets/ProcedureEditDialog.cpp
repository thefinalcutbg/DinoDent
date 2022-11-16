#include "ProcedureEditDialog.h"
#include "Presenter/ProcedureEditorPresenter.h"
#include "Presenter/ObturationPresenter.h"
#include "Presenter/CrownPresenter.h"
#include "Presenter/ImplantPresenter.h"


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

	ui.errorLabel->setStyleSheet("color:red");
	ui.crownWidget->ui.rangeWidget->setErrorLabel(ui.errorLabel);
	ui.obturWidget->ui.surfaceSelector->setErrorLabel(ui.errorLabel);
	ui.fiberWidget->ui.rangeWidget->setErrorLabel(ui.errorLabel);
	ui.commonWidget->ui.dateEdit->setErrorLabel(ui.errorLabel);
	ui.commonWidget->ui.diagnosisEdit->setErrorLabel(ui.errorLabel);
	ui.commonWidget->ui.manipulationEdit->setErrorLabel(ui.errorLabel);

}

ProcedureEditDialog::~ProcedureEditDialog()
{
}

void ProcedureEditDialog::setMtype(ProcedureType m)
{


	switch (m)
	{
	case ProcedureType::obturation:
		ui.stackedWidget->setCurrentIndex(1);
		break;
	case ProcedureType::crown:
	case ProcedureType::bridge:
		ui.stackedWidget->setCurrentIndex(2);
		break;
	case ProcedureType::implant:
		ui.stackedWidget->setCurrentIndex(3);
		break;
	case ProcedureType::fibersplint:
		ui.stackedWidget->setCurrentIndex(4);
		break;
	default:
		ui.stackedWidget->hide();
		setFixedWidth(311);
		setFixedHeight(311);
		break;

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

IFiberSplintView* ProcedureEditDialog::fiberView()
{
	return ui.fiberWidget;
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
