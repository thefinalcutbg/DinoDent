#include "ProcedureEditorPresenter.h"
#include "View/Interfaces/IProcedureInput.h"
#include "View/ModalDialogBuilder.h"
#include "Model/User.h"

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
	
	IProcedureInput::CommonData data;

	data.diagnosis = result->diagnosis;
	data.financingSource = result->financingSource;
	data.notes = result->notes;

	auto inputView = view->procedureInput();

    inputView->setCommonData(data, result->financingSource == FinancingSource::NHIF);
	
	switch (result->getScope()) {

		case ProcedureScope::AllOrNone:
		{
			std::holds_alternative<AnesthesiaMinutes>(result->param) ?
				inputView->setParameterData(std::get<AnesthesiaMinutes>(result->param).minutes)
				:
				inputView->setParameterData();
		}
		break;

		case ProcedureScope::SingleTooth:
		{
			std::holds_alternative<RestorationData>(result->param) ?
				inputView->setParameterData(
					result->getToothIndex().supernumeral,
					std::get<RestorationData>(result->param)
				)
				:
				inputView->setParameterData(result->getToothIndex().supernumeral);
		}
		break;

		case ProcedureScope::Range:
		{
			bool allowSingleRange =
				m_code.type() == ProcedureType::RemoveCrownOrBridge
				|| m_code.type() == ProcedureType::Denture;

			inputView->setParameterData(
				std::get<ConstructionRange>(result->affectedTeeth),
                allowSingleRange
			);
		}


	}

	view->procedureInput()->dateEdit()->validateInput();

	if (!m_code.isLegacy()) {
		view->procedureInput()->disablePost();
	}

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

	auto data = view->procedureInput()->getResult();

    if (m_code.isNhifOnly()
		&& data.financingSource != FinancingSource::NHIF
		&& !ModalDialogBuilder::askDialog(
			"Посоченият източник на финансиране е различен от НЗОК "
			"и процедурата няма да бъде включена в месечния отчет."
			" Желаете ли да продължите?"
		)
		)
	{
		return;
	}

	//procedure creator:
	result.emplace(Procedure{});

	result->code = m_code;
	result->date = view->procedureInput()->dateEdit()->getDate();
	result->his_index = m_hisIndex;
	result->notes = data.notes;
	result->diagnosis = data.diagnosis;
	result->financingSource = data.financingSource;
	result->LPK = User::doctor().LPK;

	//range
	if (std::holds_alternative<ConstructionRange>(data.parameters)) {
		result->affectedTeeth = std::get<ConstructionRange>(data.parameters);
	}
	//anesthesia
	else if (std::holds_alternative<int>(data.parameters)) {
		result->param = AnesthesiaMinutes(std::get<int>(data.parameters));
	}
	//restoration
	else if (std::holds_alternative<std::pair<bool, RestorationData>>(data.parameters)) {

		auto pair = std::get<std::pair<bool, RestorationData >>(data.parameters);
		result->param = pair.second;
		m_tooth_index.supernumeral = pair.first;
		result->affectedTeeth = m_tooth_index;
	}
	//tooth specific
	else if (std::holds_alternative<bool>(data.parameters)) {
		m_tooth_index.supernumeral = std::get<bool>(data.parameters);
		result->affectedTeeth = m_tooth_index;
	}

	view->closeDialog();
}

