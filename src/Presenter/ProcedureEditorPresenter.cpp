#include "ProcedureEditorPresenter.h"
#include "View/Interfaces/ICommonFields.h"
#include "Model/Dental/KSMP.h"
ProcedureEditorPresenter::ProcedureEditorPresenter(const Procedure& m, const Date& patientTurns18)
	: 
	m_procedure(m),
	view(nullptr),
	_dateValidator(patientTurns18),
	commonEditorPresenter(m)
{
	_dateValidator.setProcedure(m.code, m.isNhif());
}

std::optional<Procedure> ProcedureEditorPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);

	return result;
}

void ProcedureEditorPresenter::setView(IProcedureEditDialog* view)
{
	this->view = view;

	commonEditorPresenter.setView(view->commonFields());
	view->commonFields()->setCurrentPresenter(&commonEditorPresenter);
	commonEditorPresenter.setAdditionalTemplateParameters();

	view->commonFields()->setFinancingSource(m_procedure.financingSource);
	view->commonFields()->setHyperdonticState(m_procedure.hyperdontic);
	view->commonFields()->dateEdit()->setInputValidator(&_dateValidator);
	view->commonFields()->setNotes(m_procedure.notes);

	switch (m_procedure.type)
	{
		case ProcedureType::obturation:
			view->commonFields()->surfaceSelector()->setData(std::get<ProcedureObtData>(m_procedure.result));
			break;
		case ProcedureType::bridge:
		case ProcedureType::fibersplint:
		case ProcedureType::removebridgeOrSplint:
		{
			auto [begin, end] = std::get<ConstructionRange>(m_procedure.result);
			view->commonFields()->rangeWidget()->setBridgeRange(begin, end);
		}
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


	//procedure creator:

	result = commonEditorPresenter.getProcedures().at(0);

	result->notes = view->commonFields()->getNotes();

	switch (result->type)
	{
		case ProcedureType::obturation:
			result->result = view->commonFields()->surfaceSelector()->getData();
			break;
		case ProcedureType::bridge:
		case ProcedureType::fibersplint:
		case ProcedureType::removebridgeOrSplint:
		{
			auto [begin, end] = view->commonFields()->rangeWidget()->getRange();
			result->result = ConstructionRange{ begin, end };
		}
			break;
		default:
			result->result = NoData{};
	}

	view->closeDialog();
}

