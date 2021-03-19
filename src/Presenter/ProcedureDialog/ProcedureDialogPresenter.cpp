#include "ProcedureDialogPresenter.h"
#include "Model/Date.h"



ProcedureDialogPresenter::ProcedureDialogPresenter(IProcedureDialog* view) :
	manipulationList{ getManipulationTemplate() }, view(view), teeth(NULL), errorState(true)
{
	selectedTeeth.reserve(32);

}

void ProcedureDialogPresenter::indexErrorCheck()
{
	if (currentIndex == -1)
	{
		view->showErrorMessage("Моля, изберете манипулация");
		errorState = true;
		return;
	}

	auto &m = manipulationList[currentIndex];

	if (!selectedTeeth.size() && m.type != ManipulationType::bridge && m.type != ManipulationType::general)
	{
		errorState = true;

		view->showErrorMessage("За да въведете тази манипулация, трябва да сте избрали поне един зъб");

		return;
	}

	errorState = false;
}

std::vector<Manipulation> ProcedureDialogPresenter::generateManipulations()
{

	auto& m_template = manipulationList[currentIndex];

	auto product = std::move(manipulation_generator.getManipulations
	(
		DialogData
		{
					manipulationList[currentIndex],
					Date(ui.dateField->getText()),
					ui.manipulationField->getText(),
					ui.diagnosisField->getText(),
					ui.materialField->getText(),
					view->getPrice(),
					ui.surfaceSelector->getSurfaces(),
					ui.rangeBox->getRange(),
					selectedTeeth
		}

	));

	auto& failed = product.failedByToothNumber;

	if (failed.size())
	{
		std::string message;
		message.reserve(72 + failed.size()*4);

		message.append("Манипулацията не можа да бъде автоматично генерирана за следните зъби: ");

		for (int i = 0; i < failed.size(); i++)
		{
			
			int& idx = failed[i];
			message.append(std::to_string(utils.getToothNumber(idx, teeth->at(idx).temporary.exists())));

			if (i != failed.size() - 1) 
				message.append(", ");
		}

		view->showErrorDialog(message);
	}

	return product.manipulations;

}

void ProcedureDialogPresenter::setUIComponents(ProcedureDialogElements uiComp)
{
	ui = uiComp;
	ui.dateField->set_Validator(&date_validator);
	ui.rangeBox->set_Validator(&range_validator);
}

void ProcedureDialogPresenter::changeValidatorsDynamically()
{

	ui.manipulationField->set_Validator(&notEmpty_validator);
	ui.diagnosisField->set_Validator(&notEmpty_validator);
	ui.surfaceSelector->set_Validator(&notEmpty_validator);
	
	if (selectedTeeth.size() < 2){

		return;
	}

	auto& type = manipulationList[currentIndex].type;

	switch (type)
	{
		case ManipulationType::general: return;
		case ManipulationType::any: return;
		case ManipulationType::bridge: return;
	}

	ui.manipulationField->set_Validator(NULL);
	ui.diagnosisField->set_Validator(NULL);
	ui.surfaceSelector->set_Validator(NULL);
	
}

std::vector<Manipulation> ProcedureDialogPresenter::openDialog(const std::vector<Tooth*>& selectedTeeth, 
												   			    const std::array<Tooth, 32>& teeth,
																const Date& ambListDate)
{
	accepted = false;

	this->selectedTeeth = selectedTeeth;
	this->teeth = &teeth;

	view->loadManipulationList(manipulationList);

	//getting date
	date_validator.setAmbListDate(ambListDate);
	auto date = Date::getCurrentDate();

	if (!date_validator.validate(date)) {
		date = ambListDate;
	}
	ui.dateField->setFieldText(Date::toString(date));
	ui.dateField->setAppearence(true);

	//setting rangeBox
	auto range = autofill.getInitialBridgeRange(selectedTeeth);
	bridgeDiagnosis = autofill.getBridgeDiag(std::get<0>(range), std::get<1>(range), teeth);
	ui.rangeBox->setRange(std::get<0>(range), std::get<1>(range));

	//autofilling diagnosis

	if (selectedTeeth.size() == 1)
	{
		obturationDiagnosis = AutoComplete::getObturDiag(*selectedTeeth[0]);
		endoDiagnosis = AutoComplete::getEndoDiag(*selectedTeeth[0]);
		crownDiagnosis = AutoComplete::getCrownDiag(*selectedTeeth[0]);
		extractionDiagnosis = AutoComplete::getExtrDiag(*selectedTeeth[0]);
		ui.surfaceSelector->setSurfaces(autofill.getSurfaces(*selectedTeeth[0]));
	}
	else
	{
		obturationDiagnosis.clear();
		endoDiagnosis.clear();
		crownDiagnosis.clear();
		extractionDiagnosis.clear();
		ui.surfaceSelector->setSurfaces(std::array<bool, 6>{0, 0, 0, 0, 0, 0});
	}
	
	//updating view for which teeth are selected
	std::vector<int> selectedTeethNum;
	selectedTeethNum.reserve(32);

	for (const Tooth* t : selectedTeeth)
	{
		selectedTeethNum.emplace_back(utils.getToothNumber(t->index, t->temporary.exists()));
	}

	view->setSelectionLabel(selectedTeethNum);

	//starting the modal dialog while loop
	view->openProcedureDialog();

	if (accepted) {
		return generateManipulations();
	}

	return std::vector<Manipulation>{};
}

void ProcedureDialogPresenter::rangeChanged(int begin, int end)
{
	bridgeDiagnosis = autofill.getBridgeDiag(begin, end, *teeth);
	ui.diagnosisField->setFieldText(bridgeDiagnosis);

	auto& m = manipulationList[currentIndex];

	view->setParameters(m.price * (end - begin + 1));
}

void ProcedureDialogPresenter::indexChanged(int index)
{
	currentIndex = index;
	
	indexErrorCheck();

	if(errorState) return;

	auto& m = manipulationList[currentIndex];

	ui.manipulationField->setFieldText(m.name);
	ui.materialField->setFieldText(m.material);
	view->setParameters(m.price);
	
		switch (m.type)
		{
			case ManipulationType::obturation:
			{
				view->setView(FormView::surface);
				ui.diagnosisField->setFieldText(obturationDiagnosis);
				break;
			}
			case ManipulationType::endo:
			{
				view->setView(FormView::material);
				ui.diagnosisField->setFieldText(endoDiagnosis);
				break;
			}
			case ManipulationType::crown:
			{
				view->setView(FormView::material);
				ui.diagnosisField->setFieldText(crownDiagnosis);
				break;
			}
			case ManipulationType::extraction:
			{
				view->setView(FormView::default);
				ui.diagnosisField->setFieldText(extractionDiagnosis);
				break;
			}
			case ManipulationType::bridge:
			{
				view->setView(FormView::bridge);
				ui.diagnosisField->setFieldText(bridgeDiagnosis);
				auto range = ui.rangeBox->getRange();
				rangeChanged(std::get<0>(range), std::get<1>(range));
				break;
			}
			default:
				view->setView(FormView::default);	
				ui.diagnosisField->setFieldText(m.diagnosis);
		}

		changeValidatorsDynamically();
}

void ProcedureDialogPresenter::diagnosisChanged(std::string diagnosis)
{
	auto t = manipulationList[currentIndex].type;

	switch (t)
	{
	case ManipulationType::bridge:
		bridgeDiagnosis = diagnosis;
		break;
	case ManipulationType::obturation:
		obturationDiagnosis = diagnosis;
		break;
	case ManipulationType::crown:
		crownDiagnosis = diagnosis;
		break;
	case ManipulationType::extraction:
		extractionDiagnosis = diagnosis;
		break;
	case ManipulationType::endo:
		endoDiagnosis = diagnosis;
		break;
	}
}


void ProcedureDialogPresenter::formAccepted()
{
	if (errorState) return;

	std::array<AbstractUIElement*, 4> elementCheck
	{
		ui.manipulationField,
		ui.diagnosisField,
		ui.dateField,
		NULL
	};


	int checkSize = 4;

	switch (manipulationList[currentIndex].type)
	{
	case ManipulationType::bridge:
		elementCheck[3] = ui.rangeBox;
		break;
	case ManipulationType::obturation:
		elementCheck[3] = ui.surfaceSelector;
		break;
	default:
		checkSize = 3; //not checking range or surface
	}

	for (int i = 0; i < checkSize; i++)
	{
		elementCheck[i]->forceValidate();

		if (!elementCheck[i]->isValid())
		{
			elementCheck[i]->setFocusAndSelectAll();

			if (i == 2) {
				view->showErrorDialog("Датата на манипулацията не може да бъде по-малка от тази на амбулаторния лист, или от различен месец!");
			}
			return;
		}
	}

	accepted = true;

	view->close();
}
