#pragma once
#include <string>
#include "TimeLapse.h"
#include "DoseQuantity.h"
#include "Route.h"
#include "WhenToTake.h"

struct Dosage
{
	//int sequence <- deduced when in container

	bool asNeeded{ false };
	
	Route route;

	DoseQuantity doseQuantity;

	unsigned int frequency{ 1 };

	TimeLapse period{ 1 };

	TimeLapse bounds{ 0 };

	WhenToTake when;

	std::string additionalInstructions;

	std::string parse() const;


};
