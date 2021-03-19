#pragma once
#include "Manipulation.h"
#include "Model/Tooth/Tooth.h"
#include "AutoComplete.h"
#include <vector>
#include <tuple>

struct ManipulationFactoryProducts
{
	std::vector<Manipulation> manipulations;
	std::vector<int> failedByToothNumber;
};

class ManipulationFactory
{


protected:

	std::string(*getDiagnosis)(const Tooth& tooth);


	bool checkDiagnosis(const Manipulation& m);
	Manipulation tryAutoComplete(Tooth* tooth, Manipulation m);

	virtual void virtualAutoComplete(Tooth* tooth, Manipulation& m) {};
	virtual bool checkManipulationValidity(const Manipulation& m);

public:
	ManipulationFactory(std::string(*getDiagnosis)(const Tooth& tooth));
	ManipulationFactoryProducts getManipulations(const std::vector<Tooth*>& selectedTeeth, Manipulation m);
};


class ObturationFactory : public ManipulationFactory
{
	bool checkSurfaces(const Manipulation& m);

private:

	bool checkManipulationValidity(const Manipulation& m) override;
	void virtualAutoComplete (Tooth* tooth, Manipulation& m) override;
public:
	ObturationFactory();

};