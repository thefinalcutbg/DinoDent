#pragma once

/*
	This class takes some of the responsibilities of the CrownPresenter,
	concerning the complicated bridge logic and improves its readability
*/

#include <vector>
#include "Model/Validator/ProcedureValidators.h"

class ToothContainer;
class Tooth;

class BridgeHelper {

	const ToothContainer& teeth;
	const std::vector<Tooth*>& selectedTeeth;
	BridgeRangeValidator validator;

	int begin{ 0 };
	int end{ 0 };
	std::string diagnosis;
	double crown_price {0};
	double bridge_price {0};
	std::string range;


	void getInitialBridgeRange();

	//when range changes:
	void refreshDiagnosis();
	void refreshRangeString();

public:

	BridgeHelper(const ToothContainer& t, const std::vector<Tooth*>& selT);

	void setPrice(double crownPrice);
	void setRange(int begin, int end);

	int Begin() { return begin; }
	int End() { return end; }
	double price() { return bridge_price; }
	const std::string& rangeString() { return range; }
	const std::string& Diagnosis() { return diagnosis; }
	
};

