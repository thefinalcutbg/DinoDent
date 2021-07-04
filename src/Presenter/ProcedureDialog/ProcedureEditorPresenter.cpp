#include "ProcedureEditorPresenter.h"
#include "View/ProcedureDialog/Widgets/CommonFields/ICommonFields.h"

ProcedureEditorPresenter::ProcedureEditorPresenter(const Procedure& m, const Date& ambDate, const Date& patientTurns18)
	: 
	m(m), 
	view(nullptr),
	_dateValidator(ambDate, patientTurns18),
	_validatableElements{nullptr}
{
	_dateValidator.setProcedure(m.code, m.nzok);

	for (auto& e : _validatableElements)
		e = nullptr;
}

Procedure ProcedureEditorPresenter::openDialog()
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

	view->commonFields()->diagnosisEdit()->setInputValidator(&_emptyValidator);
	view->commonFields()->manipulationEdit()->setInputValidator(&_emptyValidator);
	view->commonFields()->dateEdit()->setInputValidator(&_dateValidator);

	_validatableElements[0] = view->commonFields()->manipulationEdit();
	_validatableElements[1] = view->commonFields()->diagnosisEdit();
	_validatableElements[2] = view->commonFields()->dateEdit();

	

	if (m.nzok)
	{
		view->commonFields()->priceEdit()->disable(true);
		view->commonFields()->manipulationEdit()->disable(true);
	}

	view->crownView()->rangeWidget()->disbleBridgeSwitch(true);

	switch (m.type)
	{
		case ProcedureType::obturation:
			view->obturationView()->setData(std::get<ObturationData>(m.result));
			view->obturationView()->surfaceSelector()->setInputValidator(&_surfValidator);
			_validatableElements[3] = view->obturationView()->surfaceSelector();
			break;
		case ProcedureType::implant:
			view->implantView()->setData(std::get<ImplantData>(m.result));
			break;
		case ProcedureType::crown:
			view->crownView()->setData(std::get<CrownData>(m.result));
			view->crownView()->rangeWidget()->disable(true);
			break;
		case ProcedureType::bridge:
			view->crownView()->setData(std::get<BridgeData>(m.result));
			view->crownView()->rangeWidget()->setInputValidator(&_bridgeValidator);
			_validatableElements[3] = view->crownView()->rangeWidget();
			break;
	}
}

void ProcedureEditorPresenter::okPressed()
{
	//validation:
	
	for (auto& v : _validatableElements)
	{
		if (v == nullptr) continue;

		v->validateInput();

		if (v->isValid() == false)
		{
			v->setFocus();
			return;
		}
	}

	//procedure creator:

	m.date = view->commonFields()->dateEdit()->getDate();
	m.diagnosis = view->commonFields()->diagnosisEdit()->getText();
	m.name = view->commonFields()->manipulationEdit()->getText();
	m.price = view->commonFields()->priceEdit()->get_Value();

	switch (m.type)
	{
		case ProcedureType::obturation:
			m.result = view->obturationView()->getData();
			break;
		case ProcedureType::implant:
			m.result = view->implantView()->getData();
			break;
		case ProcedureType::crown:
			m.result = view->crownView()->getData();
			break;
		case ProcedureType::bridge:
			auto [begin, end] = view->crownView()->rangeWidget()->getRange();
			m.result = BridgeData{ view->crownView()->getData(), begin, end };
			break;
	}

	view->closeDialog();
}
