#pragma once
#include <vector>
#include <array>
#include "Model/Manipulation/Manipulation.h"
#include "../uiComponents/AbstractLineEdit.h"
#include "../uiComponents/AbstractRangeEdit.h"
#include "../uiComponents/DateLineEdit.h"
#include "../uiComponents/AbstractSurfaceSelector.h"


enum class FormView { default, surface, bridge, material };

struct ProcedureDialogElements
{
	ProcedureDialogElements() : 
		dateField(NULL), 
		manipulationField(NULL), 
		diagnosisField(NULL), 
		materialField(NULL),
		surfaceSelector(NULL),
		rangeBox(NULL) {}

	AbstractLineEdit* dateField;
	AbstractLineEdit* manipulationField;
	AbstractLineEdit* diagnosisField;
	AbstractLineEdit* materialField;
	//price
	AbstractSurfaceSelector* surfaceSelector;
	AbstractRangeEdit* rangeBox;
	
};


class IProcedureDialog
{
public:
	virtual void loadManipulationList(std::vector<ManipulationTemplate> manipulationList) = 0;
	virtual void openProcedureDialog() = 0;
	virtual void close() = 0;
	virtual void showErrorMessage(const std::string& error) = 0;
	virtual void showErrorDialog(const std::string& error) = 0;
	virtual void setView(ManipulationType t) = 0;

	virtual void setParameters(double price) = 0;
	virtual void setParameters(const std::string& material) = 0;
	virtual void setSelectionLabel(const std::vector<int>& selectedTeethNum) = 0;
	virtual double getPrice() = 0;

	
};