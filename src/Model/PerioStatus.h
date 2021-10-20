#pragma once

constexpr int upperCount = 0;
constexpr int lingualOffset = 96;
constexpr int perioSurfCount = 192;

struct PerioStatus
{
	bool disabled[32] { false };
    int cal[perioSurfCount] { 0 };
	int pd[perioSurfCount] { 0 };
	int gm[perioSurfCount]{ 0 }; //don't forget to intialize!
	bool FMBS[128]{ false };
	bool FMPS[128]{ false };
	bool bop[perioSurfCount] { false };
	int ag[64] { 0 };
	int mobility[32] { 0 };

};