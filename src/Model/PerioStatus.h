#pragma once

constexpr int upperCount = 0;
constexpr int perioSurfCount = 192;
constexpr int scoreSurfCount = 128;

struct PerioStatus
{
	
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

