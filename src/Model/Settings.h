#pragma once

struct Settings //only practice-specific settings
{
	bool getHisHistoryAuto{ false };
	bool getPisHistoryAuto{ false };
	bool getNraStatusAuto{ false };
	bool getHirbNoAuto{ false };
	bool nhifDailyLimitCheck{ false };
	bool nhifWeekendCheck{ false };
	bool autoStatus{ false };
	int timeout{ 15 }; //in seconds!
};