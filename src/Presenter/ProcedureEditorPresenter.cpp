#include "ProcedureEditorPresenter.h"
#include "View/Interfaces/ICommonFields.h"
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

	view->commonFields()->dateEdit()->set_Date(result->date);
	view->commonFields()->setFinancingSource(result->financingSource);
	view->commonFields()->setHyperdonticState(result->hyperdontic);
	view->commonFields()->dateEdit()->setInputValidator(&_dateValidator);
	view->commonFields()->setNotes(result->notes);
	view->commonFields()->diagnosisEdit()->set_Text(result->diagnosis);

	switch (result->code.type())
	{
		case ProcedureType::general:
			view->commonFields()->setLayout(ICommonFields::General);
			break;
		case ProcedureType::obturation:
			view->commonFields()->setLayout(ICommonFields::Restoration);
			view->commonFields()->surfaceSelector()->setData(std::get<ProcedureObtData>(result->result));
			view->commonFields()->surfaceSelector()->setInputValidator(&surface_validator);
			break;
		case ProcedureType::bridge:
		case ProcedureType::fibersplint:
		{
			view->commonFields()->rangeWidget()->setInputValidator(&range_validator);
			view->commonFields()->setLayout(ICommonFields::Range);
			auto [begin, end] = std::get<ConstructionRange>(result->result);
			view->commonFields()->rangeWidget()->setBridgeRange(begin, end);
		}
			break;
		case ProcedureType::removebridgeOrSplint:
		{
			view->commonFields()->setLayout(ICommonFields::Range);
			auto [begin, end] = std::get<ConstructionRange>(result->result);
			view->commonFields()->rangeWidget()->setBridgeRange(begin, end);
		}
			break;
		default:
			view->commonFields()->setLayout(ICommonFields::ToothSpecific);
			break;
	}

	view->commonFields()->dateEdit()->validateInput();

	result.reset();
}

void ProcedureEditorPresenter::okPressed()
{
	//validation:
	std::vector<AbstractUIElement*> validatable{
		view->commonFields()->dateEdit(),
		view->commonFields()->diagnosisEdit(),
		view->commonFields()->surfaceSelector(),
		view->commonFields()->rangeWidget()
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
	result->notes = view->commonFields()->getNotes();
	result->date = view->commonFields()->dateEdit()->getDate();
	result->diagnosis = view->commonFields()->diagnosisEdit()->getText();
	result->financingSource = view->commonFields()->getFinancingSource();
	result->hyperdontic = view->commonFields()->onHyperdontic();

	switch (result->code.type())
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

