#pragma once

struct Settings
{
	bool getHisHistoryAuto{ false };
	bool getPisHistoryAuto{ false };
	bool getNraStatusAuto{ false };
	bool nhifDailyLimitCheck{ false };
	bool nhifWeekendCheck{ false };
	int timeout{ 15 }; //in seconds!
};