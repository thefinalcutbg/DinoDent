#include "ProcedureEditorPresenter.h"
#include "View/Interfaces/IProcedureInput.h"
#include "View/ModalDialogBuilder.h"

ProcedureEditorPresenter::ProcedureEditorPresenter(const Procedure& p, const Date& patientTurns18)
	:
	view(nullptr),
	_dateValidator(patientTurns18)
{

	result.emplace(Procedure());
	
	//parameters not set to view
	m_code = p.code;
	m_tooth_index = p.getToothIndex();
	m_hisIndex = p.his_index;

	//parameters which are set to view
	result->code = p.code;
	result->date = p.date;
	result->diagnosis = p.diagnosis;
	result->param = p.param;
	result->financingSource = p.financingSource;
	result->notes = p.notes;
	result->diagnosis.description = p.diagnosis.description;
	result->affectedTeeth = p.affectedTeeth;

	_dateValidator.setProcedure(result->code.oldCode(), result->financingSource == FinancingSource::NHIF);
}

std::optional<Procedure> ProcedureEditorPresenter::openDialog()
{
    ModalDialogBuilder::openDialog(*this);

	return result;
}

void ProcedureEditorPresenter::setView(IProcedureEditDialog* view)
{
	this->view = view;

	view->procedureInput()->dateEdit()->set_Date(result->date);
	view->procedureInput()->setFinancingSource(result->financingSource);
	view->procedureInput()->setHyperdonticState(result->getToothIndex().supernumeral);
	view->procedureInput()->dateEdit()->setInputValidator(&_dateValidator);
	view->procedureInput()->setNotes(result->notes);
	view->procedureInput()->diagnosisEdit()->set_Text(result->diagnosis.description);
	view->procedureInput()->diagnosisCombo()->setIndex(result->diagnosis.index());
	
	if (result->diagnosis.index()) {
		view->procedureInput()->diagnosisEdit()->setInputValidator(nullptr);
	}

	range_validator.allowSingleRange = result->code.type() == ProcedureType::Denture;

	//GENERAL
	if (result->code.isGeneral()) {

		view->procedureInput()->surfaceSelector()->setInputValidator(nullptr);
		view->procedureInput()->rangeWidget()->setInputValidator(nullptr);

		result->code.isAnesthesia() ?
			view->procedureInput()->setLayout(IProcedureInput::Anesthesia)
			:
			view->procedureInput()->setLayout(IProcedureInput::General);

	}

	//TOOTH SPECIFIC
	if (result->code.isToothSpecific()) {

		view->procedureInput()->rangeWidget()->setInputValidator(nullptr);

		if (result->code.isRestoration()) {

			view->procedureInput()->setLayout(IProcedureInput::Restoration);
			view->procedureInput()->surfaceSelector()->setInputValidator(&surface_validator);
		}
		else {
			view->procedureInput()->setLayout(IProcedureInput::ToothSpecific);
		}

	}

	//RANGE SPECIFIC
	if (result->code.isRangeSpecific()) {

		view->procedureInput()->setLayout(IProcedureInput::Range);
		view->procedureInput()->surfaceSelector()->setInputValidator(nullptr);

		view->procedureInput()->rangeWidget()->setInputValidator(

			m_code.requiresRangeValidation() ?
				&range_validator
				:
				nullptr
		);
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
            e->set_focus();
			return;
		}
	}

	//procedure creator:
	result.emplace(Procedure{});
	result->code = m_code;
	result->notes = view->procedureInput()->getNotes();
	result->date = view->procedureInput()->dateEdit()->getDate();
	result->diagnosis = view->procedureInput()->diagnosisCombo()->getIndex();
	result->diagnosis.description = view->procedureInput()->diagnosisEdit()->getText();
	result->financingSource = view->procedureInput()->getFinancingSource();
	result->his_index = m_hisIndex;

	if (result->code.isGeneral()) {

		if (result->code.isAnesthesia()) {
			result->param = AnesthesiaMinutes{ view->procedureInput()->minutes() };
		}

		result->affectedTeeth = std::monostate{};
	}
	else if (result->code.isToothSpecific()) {

		if (result->code.isRestoration()) {

			result->param = view->procedureInput()->surfaceSelector()->getData();
		}

		m_tooth_index.supernumeral = view->procedureInput()->onHyperdontic();

		result->affectedTeeth = m_tooth_index;
	}
	else if (result->code.isRangeSpecific()) {

		auto [begin, end] = view->procedureInput()->rangeWidget()->getRange();

		result->affectedTeeth = ConstructionRange{ begin, end };
	}

	view->closeDialog();
}

