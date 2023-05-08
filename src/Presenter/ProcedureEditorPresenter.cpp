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
	m_hisIndex = p.his_index;

	result->code = p.code;
	result->date = p.date;
	result->hyperdontic = p.hyperdontic;
	result->diagnosis = p.diagnosis;
	result->result = p.result;
	result->financingSource = p.financingSource;
	result->notes = p.notes;
	result->diagnosis.additionalDescription = p.diagnosis.additionalDescription;

	_dateValidator.setProcedure(result->code.oldCode(), result->financingSource == FinancingSource::NHIF);
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
	view->procedureInput()->diagnosisEdit()->set_Text(result->diagnosis.additionalDescription);
	view->procedureInput()->diagnosisCombo()->setIndex(result->diagnosis.index());

	if (result->diagnosis.index()) {
		view->procedureInput()->diagnosisEdit()->setInputValidator(nullptr);
	}

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
		case ProcedureType::anesthesia:
			view->procedureInput()->setLayout(IProcedureInput::Anesthesia);
			view->procedureInput()->setMinutes(std::get<AnesthesiaMinutes>(result->result).minutes);
			break;
		case ProcedureType::bridge:
		case ProcedureType::fibersplint:
		case ProcedureType::denture:
		{
			view->procedureInput()->rangeWidget()->setInputValidator(&range_validator);
			view->procedureInput()->setLayout(IProcedureInput::Range);
			auto [begin, end] = std::get<ConstructionRange>(result->result);
			view->procedureInput()->rangeWidget()->setBridgeRange(begin, end);
		}
			break;
			/*
		case ProcedureType::removebridgeOrSplint:
		{
			view->procedureInput()->setLayout(IProcedureInput::Range);
			auto [begin, end] = std::get<ConstructionRange>(result->result);
			view->procedureInput()->rangeWidget()->setBridgeRange(begin, end);
		}
		*/
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
	//bool validIdx = view->procedureInput()->diagnosisCombo()->getIndex();
	
	//view->procedureInput()->diagnosisEdit()->setInputValidator(validIdx ? nullptr : &not_emptyValidator);


	//validation:
	std::vector<AbstractUIElement*> validatable{
		view->procedureInput()->dateEdit(),
		view->procedureInput()->surfaceSelector(),
		view->procedureInput()->rangeWidget(),
		view->procedureInput()->diagnosisEdit()
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
	result->diagnosis = view->procedureInput()->diagnosisCombo()->getIndex();
	result->diagnosis.additionalDescription = view->procedureInput()->diagnosisEdit()->getText();
	result->financingSource = view->procedureInput()->getFinancingSource();
	result->hyperdontic = view->procedureInput()->onHyperdontic();
	result->tooth = m_tooth;
	result->his_index = m_hisIndex;

	switch (result->code.type())
	{
		case ProcedureType::obturation:
			result->result = view->procedureInput()->surfaceSelector()->getData();
			break;
		case ProcedureType::anesthesia:
			result->result = AnesthesiaMinutes{ view->procedureInput()->minutes() };
			break;
		case ProcedureType::bridge:
		case ProcedureType::fibersplint:
		//case ProcedureType::removebridgeOrSplint:
		case ProcedureType::denture:
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

