#pragma once

constexpr int upperCount = 0;
constexpr int perioSurfCount = 192;
constexpr int scoreSurfCount = 128;

struct PerioStatus
{
	
	int smoker{ 0 };
	int boneLoss{ 0 };

	bool completeRestorationNeeded{ false };
	bool systemic{ false };

	int furcation[96]{ 0 };
	bool disabled[32] { false };
    int cal[perioSurfCount] { 0 };
	int pd[perioSurfCount] { 0 };
	bool FMBS[scoreSurfCount]{ false };
	bool FMPS[scoreSurfCount]{ false };
	bool bop[perioSurfCount] { false };
	int ag[64] { 0 };
	int mobility[32] { 0 };

};

