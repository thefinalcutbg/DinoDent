#pragma once
#include "Manipulation.h"
#include "Model/Tooth/Tooth.h"
#include "ManipulationFactory.h"
#include <vector>


struct DialogData
{
	const ManipulationTemplate& mTemplate; 
	const Date& date; 
	const std::string& mName; 
	const std::string& mDiagnosis; 
	const std::string& mMaterial; 
	double price; 
	const std::array<bool, 6>& surfaces; 
	const std::tuple<int, int> range; 
	const std::vector<Tooth*>& selectedTeeth;
};

class ManipulationGenerator
{
	ObturationFactory obturation_factory;
	ManipulationFactory extraction_factory;
	ManipulationFactory crown_factory;
	ManipulationFactory endo_factory;
	

public:
	ManipulationGenerator();
	ManipulationFactoryProducts getManipulations(DialogData data);

};

