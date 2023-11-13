#pragma once

#include "Model/Date.h"

struct Hospitalization {

	enum Status { Unknown, Planned, Active, Terminated, Completed };

	Date authoredOn;
	Status status;

};
