#pragma once
#include <string>
#include "TimeLapse.h"
#include "DoseQuantity.h"
#include "Route.h"
struct Dosage
{
	//int sequence <- deduced when in container

	bool asNeeded{ false };
	
	Route route;

	DoseQuantity doseQuantity;

	unsigned int frequency;

	TimeLapse period;

	TimeLapse duration;

	std::vector<int> whenToTake; //numenclature

	unsigned int offset;

	std::string frequencySuffix()
	{
		if (frequency != 1) return u8" пъти на ";
		
		return u8" път на ";
	}

	std::string interpretation();


};
