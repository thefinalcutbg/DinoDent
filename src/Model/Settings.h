#pragma once

struct Settings //only practice-specific settings
{
	bool getHisHistoryAuto{ false };
	bool getPisHistoryAuto{ false };
	bool getNraStatusAuto{ false };
	bool nhifDailyLimitCheck{ false };
	bool nhifWeekendCheck{ false };
	int timeout{ 15 }; //in seconds!
};