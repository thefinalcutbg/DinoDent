#include "ProcedureDialogPresenter.h"
#include "Model/Date.h"



ProcedureDialogPresenter::ProcedureDialogPresenter
	(
		const std::vector<ManipulationTemplate>& mList,
		const std::vector<Tooth*>& selectedTeeth, 
		const std::array<Tooth, 32>& teeth, 
		const Date& ambListDate
	) 
	:
	manipulationList{ mList }, 
	manipulations{},
	view(NULL), 
	teeth(&teeth), 
	selectedTeeth(selectedTeeth),
	errorState(true)
{
	date_validator.setAmbListDate(ambListDate);

	if (selectedTeeth.size() == 1)
	{
		diagnosisMap[ManipulationType::obturation] = AutoComplete::getObturDiag(*selectedTeeth[0]);
		diagnosisMap[ManipulationType::endo] = AutoComplete::getEndoDiag(*selectedTeeth[0]);
		diagnosisMap[ManipulationType::crown] = AutoComplete::getCrownDiag(*selectedTeeth[0]);
		diagnosisMap[ManipulationType::extraction] = AutoComplete::getExtrDiag(*selectedTeeth[0]);
	}

	auto range = AutoComplete::getInitialBridgeRange(selectedTeeth);
	diagnosisMap[ManipulationType::bridge] = autofill.getBridgeDiag(std::get<0>(range), std::get<1>(range), teeth);
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

void ProcedureDialogPresenter::setView(IProcedureDialog* view, ProcedureDialogElements uiComp)
{
	this->view = view;

	ui = uiComp;

	ui.dateField->set_Validator(&date_validator);
	ui.rangeBox->set_Validator(&range_validator);

	view->loadManipulationList(manipulationList);

	auto date = Date::getCurrentDate();

	if (!date_validator.validate(date)) {
		date = date_validator.getMin();
	}
	ui.dateField->setFieldText(Date::toString(date));

	auto range = autofill.getInitialBridgeRange(selectedTeeth);
	ui.rangeBox->setRange(std::get<0>(range), std::get<1>(range));

	if (selectedTeeth.size() == 1)
	{
		ui.surfaceSelector->setSurfaces(AutoComplete::getSurfaces(*selectedTeeth[0]));
	}

	std::vector<int> selectedTeethNum;
	selectedTeethNum.reserve(32);

	for (const Tooth* t : selectedTeeth)
	{
		selectedTeethNum.emplace_back(utils.getToothNumber(t->index, t->temporary.exists()));
	}

	view->setSelectionLabel(selectedTeethNum);

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


void ProcedureDialogPresenter::setValidatorsDynamically()
{

	ui.manipulationField->set_Validator(&notEmpty_validator);
	ui.diagnosisField->set_Validator(&notEmpty_validator);
	ui.surfaceSelector->set_Validator(&surface_validator);
	
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

void ProcedureDialogPresenter::rangeChanged(int begin, int end)
{
	diagnosisMap[ManipulationType::bridge] = autofill.getBridgeDiag(begin, end, *teeth);
	ui.diagnosisField->setFieldText(diagnosisMap[ManipulationType::bridge]);

	auto& m = manipulationList[currentIndex];
	int length = end - begin + 1;
	view->setParameters(m.price*length);
}

void ProcedureDialogPresenter::indexChanged(int index)
{
	currentIndex = index;
	
	indexErrorCheck();

	if(errorState) return;

	auto& m = manipulationList[currentIndex];
	if (!m.diagnosis.empty())
	{
		diagnosisMap[m.type] = m.diagnosis;
	}
	ui.manipulationField->setFieldText(m.name);
	ui.materialField->setFieldText(m.material);
	ui.diagnosisField->setFieldText(diagnosisMap[m.type]);

	if (m.type == ManipulationType::bridge)
	{
		auto range = ui.rangeBox->getRange();
		int last = std::get<1>(range);
		int first = std::get<0>(range);
		view->setParameters(m.price * (last - first + 1));
	}
	else
	{
		view->setParameters(m.price);
	}
	
	view->setView(m.type);

	setValidatorsDynamically();
}


void ProcedureDialogPresenter::diagnosisChanged(std::string diagnosis)
{
	auto t = manipulationList[currentIndex].type;

	diagnosisMap[t] = diagnosis;
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

	manipulations = generateManipulations();

	view->close();
}

std::vector<Manipulation> ProcedureDialogPresenter::getManipulations()
{
	return manipulations;
}