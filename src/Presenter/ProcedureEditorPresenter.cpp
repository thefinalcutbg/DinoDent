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
	result->affectedTeeth = p.affectedTeeth;
	result->price = p.price;

	_dateValidator.setProcedure(result->code.nhifCode(), result->financingSource == FinancingSource::NHIF);
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
	view->procedureInput()->dateEdit()->setInputValidator(&_dateValidator);
	
	IProcedureInput::Data data;

	data.code = result->code;
	data.diagnosis = result->diagnosis;
	data.financingSource = result->financingSource;
	data.hyperdontic = result->getToothIndex().supernumeral;
	data.notes = result->notes;
	data.param = result->param;
	data.price = result->price;

	if (result->getScope() == ProcedureScope::Range) {
		data.range = std::get<ConstructionRange>(result->affectedTeeth);
	}

	view->procedureInput()->dateEdit()->validateInput();

	if (!m_code.isLegacy()) {
		view->procedureInput()->disablePost();
	}

	view->procedureInput()->setData(data);

	result.reset();
}

void ProcedureEditorPresenter::okPressed()
{

	if (!view->procedureInput()->dateEdit()->isValid()) {
		return;
	}

	auto validationResult = view->procedureInput()->isValid();

	if (validationResult.size()) {
		ModalDialogBuilder::showMessage(validationResult);
		return;
	}

	auto data = view->procedureInput()->getData();

	//procedure creator:
	result.emplace(Procedure{});

	result->code = m_code;
	result->date = view->procedureInput()->dateEdit()->getDate();
	result->his_index = m_hisIndex;
	
	if (m_code.getScope() != ProcedureScope::AllOrNone) {

		m_tooth_index.supernumeral = data.hyperdontic;
		result->affectedTeeth = m_tooth_index;
	}

	result->price = data.price;
	result->notes = data.notes;
	result->diagnosis = data.diagnosis;
	result->financingSource = data.financingSource;
	
	if (data.range.has_value()) {
		result->affectedTeeth = data.range.value();
	}

	result->param = data.param;

	view->closeDialog();
}

