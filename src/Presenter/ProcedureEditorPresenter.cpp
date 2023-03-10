#include "ProcedureEditorPresenter.h"
#include "View/Interfaces/IProcedureInput.h"
#include "Model/Dental/KSMP.h"
ProcedureEditorPresenter::ProcedureEditorPresenter(const Procedure& p, const Date& patientTurns18)
	:
	view(nullptr),
	_dateValidator(patientTurns18)
{

	result.emplace(Procedure());
	
	m_code = p.code;
	m_temp = p.temp;
	m_tooth = p.tooth;

	result->code = p.code;
	result->date = p.date;
	result->hyperdontic = p.hyperdontic;
	result->diagnosis = p.diagnosis;
	result->result = p.result;
	result->financingSource = p.financingSource;
	result->notes = p.notes;

	_dateValidator.setProcedure(result->code.nhifCode());
}

std::optional<Procedure> ProcedureEditorPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);

	return result;
}

void ProcedureEditorPresenter::setView(IProcedureEditDialog* view)
{
	this->view = view;

	view->procedureInput()->dateEdit()->set_Date(result->date);
	view->procedureInput()->setFinancingSource(result->financingSource);
	view->procedureInput()->setHyperdonticState(result->hyperdontic);
	view->procedureInput()->dateEdit()->setInputValidator(&_dateValidator);
	view->procedureInput()->setNotes(result->notes);
	view->procedureInput()->diagnosisEdit()->set_Text(result->diagnosis);

	switch (result->code.type())
	{
		case ProcedureType::general:
			view->procedureInput()->setLayout(IProcedureInput::General);
			break;
		case ProcedureType::obturation:
			view->procedureInput()->setLayout(IProcedureInput::Restoration);
			view->procedureInput()->surfaceSelector()->setData(std::get<ProcedureObtData>(result->result));
			view->procedureInput()->surfaceSelector()->setInputValidator(&surface_validator);
			break;
		case ProcedureType::bridge:
		case ProcedureType::fibersplint:
		{
			view->procedureInput()->rangeWidget()->setInputValidator(&range_validator);
			view->procedureInput()->setLayout(IProcedureInput::Range);
			auto [begin, end] = std::get<ConstructionRange>(result->result);
			view->procedureInput()->rangeWidget()->setBridgeRange(begin, end);
		}
			break;
		case ProcedureType::removebridgeOrSplint:
		{
			view->procedureInput()->setLayout(IProcedureInput::Range);
			auto [begin, end] = std::get<ConstructionRange>(result->result);
			view->procedureInput()->rangeWidget()->setBridgeRange(begin, end);
		}
			break;
		default:
			view->procedureInput()->setLayout(IProcedureInput::ToothSpecific);
			break;
	}

	view->procedureInput()->dateEdit()->validateInput();

	result.reset();
}

void ProcedureEditorPresenter::okPressed()
{
	//validation:
	std::vector<AbstractUIElement*> validatable{
		view->procedureInput()->dateEdit(),
		view->procedureInput()->diagnosisEdit(),
		view->procedureInput()->surfaceSelector(),
		view->procedureInput()->rangeWidget()
	};


	for (AbstractUIElement* e : validatable)
	{
		e->validateInput();
		if (!e->isValid())
		{
			e->setFocus();
			return;
		}
	}

	//procedure creator:

	result.emplace(Procedure{});
	result->code = m_code;
	result->notes = view->procedureInput()->getNotes();
	result->date = view->procedureInput()->dateEdit()->getDate();
	result->diagnosis = view->procedureInput()->diagnosisEdit()->getText();
	result->financingSource = view->procedureInput()->getFinancingSource();
	result->hyperdontic = view->procedureInput()->onHyperdontic();
	result->tooth = m_tooth;

	switch (result->code.type())
	{
		case ProcedureType::obturation:
			result->result = view->procedureInput()->surfaceSelector()->getData();
			break;
		case ProcedureType::bridge:
		case ProcedureType::fibersplint:
		case ProcedureType::removebridgeOrSplint:
		{
			auto [begin, end] = view->procedureInput()->rangeWidget()->getRange();
			result->result = ConstructionRange{ begin, end };
		}
			break;
		default:
			result->result = NoData{};
	}

	view->closeDialog();
}

