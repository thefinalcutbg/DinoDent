#pragma once

constexpr int upperCount = 0;
constexpr int lowerCount = 96;
constexpr int perioSurfCount = 192;

struct PerioStatus
{
	bool disabled[32] { false };
	short int cal[perioSurfCount] { 0 };
	short int pd[perioSurfCount] { 0 };
	bool bop[perioSurfCount] { false };
	short int ag[64] { 0 };
	short int mobility[32] { 0 };

};