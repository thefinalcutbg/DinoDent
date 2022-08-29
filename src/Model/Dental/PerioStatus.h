#pragma once
#include "Model/Date.h"
constexpr int upperCount = 0;
constexpr int perioSurfCount = 192;
constexpr int scoreSurfCount = 128;


struct PerioStatus
{
	long long rowid{ 0 };
	Date date{ Date::currentDate() };

	std::string LPK;

	int smoker{ 0 };	//пушач, непушач, по колко цигари на ден и т.н.
	int boneLoss{ 0 };	//загуба на кост в %

	bool completeRestorationNeeded{ false };	 //пълно възстановяване на захапка да/не
	bool systemic{ false };						//системни заболявания - да/не

	int furcation[96]{ 0 };				//фуркационни лезии
	bool disabled[32] { false };		//налични зъби
    int cal[perioSurfCount] { 0 };		//САL
	int pd[perioSurfCount] { 0 };		//PD
	bool FMBS[scoreSurfCount]{ false }; //Full Mouth Bleeding Score
	bool FMPS[scoreSurfCount]{ false }; //Full Mouth Plaque Score
	bool bop[perioSurfCount] { false }; //Bleeding on probing
	int ag[64] { 0 };					//Attached gingiva
	int mobility[32] { 0 };				//Mobility

};


struct PerioWithDisabled {

	PerioWithDisabled(const PerioStatus& p) {
		for (int i = 0; i < perioSurfCount; i++) {
			
			bool disabled = p.disabled[i / 6];

			cal[i] = disabled ? 0 : p.cal[i];
			pd[i] = disabled ? 0 : p.pd[i];
			bop[i] = disabled ? 0 : p.bop[i];
		}
	}

	int cal[perioSurfCount]{ 0 };
	int pd[perioSurfCount]{ 0 };
	bool bop[perioSurfCount]{ false };

};