#pragma once

struct Settings
{
	bool getHisHistoryAuto{ false };
	bool getPisHistoryAuto{ false };
	bool getNraStatusAuto{ false };
	bool nhifDailyLimitCheck{ false };
	int timeout{ 15 }; //in seconds!
};