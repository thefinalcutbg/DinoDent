#pragma once
#include "SurfStatus.h"

class Hyperdontic
{
	SurfaceStatus<SurfaceChild<DentistMade> > obturation;
	SurfaceStatus<SurfaceChild<Status> > caries;

	Status healthy;
	Status temporary;
	Status periodontitis;
	Status impacted;
	Status calculus;
	Status pulpitis;
	Status lesion;
	Status fracture;
	Status root;
	DentistMade endo;
	DentistMade extraction;
	DentistMade post;
	Mobility mobility;
};