#pragma once

constexpr int upperCount = 0;
constexpr int perioSurfCount = 192;
constexpr int scoreSurfCount = 128;

struct PerioStatus
{
	int furcation[96]{ 0 };
	bool disabled[32] { false };
    int cal[perioSurfCount] { 0 };
	int pd[perioSurfCount] { 0 };
	int gm[perioSurfCount]{ 0 }; //don't forget to intialize!
	bool FMBS[scoreSurfCount]{ false };
	bool FMPS[scoreSurfCount]{ false };
	bool bop[perioSurfCount] { false };
	int ag[64] { 0 };
	int mobility[32] { 0 };

};

