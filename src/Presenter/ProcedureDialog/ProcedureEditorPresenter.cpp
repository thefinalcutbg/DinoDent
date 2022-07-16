#include "ProcedureEditorPresenter.h"
#include "View/ProcedureDialog/CommonFields/ICommonFields.h"
#include "Model/KSMP.h"
ProcedureEditorPresenter::ProcedureEditorPresenter(const Procedure& m, const Date& patientTurns18)
	: 
	m_procedure(m),
	view(nullptr),
	_dateValidator(patientTurns18),
	m_validatableElement{nullptr},
	commonEditorPresenter(m)
{
	_dateValidator.setProcedure(m.code, m.nzok);
}

std::optional<Procedure> ProcedureEditorPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);

	return result;
}

void ProcedureEditorPresenter::setView(IProcedureEditDialog* view)
{
	this->view = view;

	commonEditorPresenter.setCommonFieldsView(view->commonFields());

	view->commonFields()->dateEdit()->setInputValidator(&_dateValidator);

	view->setMtype(m_procedure.type);
	
	switch (m_procedure.type)
	{
		case ProcedureType::obturation:
			view->obturationView()->setData(std::get<ProcedureObtData>(m_procedure.result));
			//view->obturationView()->surfaceSelector()->setInputValidator(&_surfValidator);
			m_validatableElement = view->obturationView()->surfaceSelector();
			break;
		case ProcedureType::implant:
			view->implantView()->setData(std::get<ImplantData>(m_procedure.result));
			break;
		case ProcedureType::crown:
			view->crownView()->setData(std::get<CrownData>(m_procedure.result));
			view->crownView()->rangeWidget()->disable(true);
			view->crownView()->lockBridgeCheckbox();
			break;
		case ProcedureType::bridge:
			view->crownView()->setData(std::get<ProcedureBridgeData>(m_procedure.result));
			view->crownView()->rangeWidget()->setInputValidator(&_bridgeValidator);
			view->crownView()->lockBridgeCheckbox();
			m_validatableElement = view->crownView()->rangeWidget();
			break;
		case ProcedureType::fibersplint:
			view->fiberView()->setData(std::get<ProcedureFiberData>(m_procedure.result));
			view->fiberView()->rangeWidget()->setInputValidator(&_bridgeValidator);
			m_validatableElement = view->fiberView()->rangeWidget();
			break;
	}

	view->commonFields()->dateEdit()->validateInput();
}

void ProcedureEditorPresenter::okPressed()
{
	//validation:
	
	if (!commonEditorPresenter.isValid()) {
		return;
	}

	if (m_validatableElement)
	{
		m_validatableElement->validateInput();

		if (!m_validatableElement->isValid()) {
			m_validatableElement->setFocus();
			return;
		}

	}

	//procedure creator:

	result = commonEditorPresenter.getProcedures().at(0);

	switch (result->type)
	{
	case ProcedureType::obturation:
		result->result = view->obturationView()->getData();
		break;
	case ProcedureType::implant:
		result->result = view->implantView()->getData();
		break;
	case ProcedureType::crown:
		result->result = view->crownView()->getData();
		break;
	case ProcedureType::bridge:
	{
		auto [begin, end] = view->crownView()->rangeWidget()->getRange();
		result->result = ProcedureBridgeData{ begin, end, view->crownView()->getData() };
		break;
	}
	case ProcedureType::fibersplint:
		auto [begin, end] = view->fiberView()->rangeWidget()->getRange();
		result->result = ProcedureFiberData{ begin, end, view->fiberView()->getData() };
		break;
	}

	view->closeDialog();
}

