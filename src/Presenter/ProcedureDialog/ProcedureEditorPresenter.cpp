#include "ProcedureEditorPresenter.h"
#include "View/ProcedureDialog/Widgets/CommonFields/ICommonFields.h"

ProcedureEditorPresenter::ProcedureEditorPresenter(const Manipulation& m)
	: 
	m(m), 
	view(nullptr)
{
}

Manipulation ProcedureEditorPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);

	return m;
}

void ProcedureEditorPresenter::setView(IProcedureEditDialog* view)
{
	this->view = view;

	view->setMtype(m.type);
	view->commonFields()->dateEdit()->set_Date(m.date);
	view->commonFields()->diagnosisEdit()->set_Text(m.diagnosis);
	view->commonFields()->manipulationEdit()->set_Text(m.name);
	view->commonFields()->priceEdit()->set_Value(m.price);
	view->crownView()->rangeWidget()->disbleBridgeSwitch(true);

	switch (m.type)
	{
	case ManipulationType::obturation:
		view->obturationView()->setData(std::get<ObturationData>(m.result));
		break;
	case ManipulationType::implant:
		view->implantView()->setData(std::get<ImplantData>(m.result));
		break;
	case ManipulationType::crown:
		view->crownView()->setData(std::get<CrownData>(m.result));
		break;
	case ManipulationType::bridge:
		view->crownView()->setData(std::get<BridgeData>(m.result));
		view->crownView()->rangeWidget()->setInputValidator(&bridgeValidator);
		break;

	}
	
}

void ProcedureEditorPresenter::okPressed()
{
	//if(!view->isValid()) return;

	m.date = view->commonFields()->dateEdit()->getDate();
	m.diagnosis = view->commonFields()->diagnosisEdit()->getText();
	m.name = view->commonFields()->manipulationEdit()->getText();
	m.price = view->commonFields()->priceEdit()->get_Value();

	view->closeDialog();
}
