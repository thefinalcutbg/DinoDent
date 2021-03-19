#pragma once

#include <vector>

#include "View/ProcedureDialog/IProcedureDialog.h"
#include "Model/Manipulation/ManipulationTemplateParser.h"
#include "Model/Manipulation/ManipulationGenerator.h"
#include "Model/Tooth/Tooth.h"
#include "Model/Tooth/ToothUtils.h"
#include "Model/Manipulation/AutoComplete.h"
#include "Model/Validator/ManipulationValidators.h"
#include "View/uiComponents/AbstractLineEdit.h"
#include "View/uiComponents/AbstractRangeEdit.h"


class ProcedureDialogPresenter
{
	IProcedureDialog* view;
	ProcedureDialogElements ui;

	ManipulationGenerator manipulation_generator;

	std::vector<Tooth*> selectedTeeth;
	const std::array<Tooth, 32>* teeth;
	std::vector<ManipulationTemplate> manipulationList;

	ToothUtils utils;
	AutoComplete autofill;

	DateValidator date_validator;
	BridgeRangeValidator range_validator;
	SurfaceValidator surface_validator;
	EmptyFieldValidator notEmpty_validator;

	std::tuple<int, int> currentRange;

	int currentIndex{-1};
	bool errorState;
	bool accepted{ false };

	std::string obturationDiagnosis;
	std::string endoDiagnosis;
	std::string extractionDiagnosis;
	std::string crownDiagnosis; 
	std::string bridgeDiagnosis; //changed dinamically

	void indexErrorCheck();
	std::vector<Manipulation> generateManipulations();
	void changeValidatorsDynamically();

	public:
		ProcedureDialogPresenter(IProcedureDialog* view);
		void setUIComponents(ProcedureDialogElements p);
		void indexChanged(int index);
		void diagnosisChanged(std::string diagnosis);
		std::vector<Manipulation> openDialog(const std::vector<Tooth*>& selectedTeeth, const std::array<Tooth, 32>& teeth, const Date& ambListDate);
		void rangeChanged(int begin, int end);
		void formAccepted();
};

