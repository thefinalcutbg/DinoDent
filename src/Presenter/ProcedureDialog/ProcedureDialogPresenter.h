#pragma once
#include "View/ProcedureDialog/IProcedureDialog.h"
#include "Model/Manipulation/ManipulationGenerator.h"
#include "Model/Tooth/ToothUtils.h"
#include "Model/Manipulation/AutoComplete.h"
#include "Model/Validator/ManipulationValidators.h"
#include "View/uiComponents/AbstractLineEdit.h"
#include "View/uiComponents/AbstractRangeEdit.h"

#include <vector>
#include <unordered_map>

class ProcedureDialogPresenter
{
	IProcedureDialog* view;
	ProcedureDialogElements ui;

	ManipulationGenerator manipulation_generator;
	std::vector<Manipulation> manipulations;
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

	std::map<ManipulationType, std::string> diagnosisMap;

	void indexErrorCheck();
	std::vector<Manipulation> generateManipulations();
	void setValidatorsDynamically();

	public:
		ProcedureDialogPresenter(const std::vector<ManipulationTemplate>& mList, const std::vector<Tooth*>& selectedTeeth, const std::array<Tooth, 32>& teeth, const Date& ambListDate);
		void setView(IProcedureDialog* view, ProcedureDialogElements p);
		
		void indexChanged(int index);
		void diagnosisChanged(std::string diagnosis);
		void rangeChanged(int begin, int end);
		void formAccepted();
		std::vector<Manipulation> getManipulations();
};

