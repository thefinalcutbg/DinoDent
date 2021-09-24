#pragma once

constexpr int upperCount = 0;
constexpr int lowerCount = 48;
constexpr int perioSurfCount = 96;


struct PerioStatus
{
	bool disabled[16] { false };
	short int cal[perioSurfCount] { 0 };
	short int pd[perioSurfCount] { 0 };
	bool bop[perioSurfCount] { false };
	short int ag[32] { 0 };
	short int mobility[16] { 0 };

};