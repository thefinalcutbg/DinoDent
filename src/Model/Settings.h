#pragma once
#include <vector>
#include <string>

struct Settings //only practice-specific settings
{
	struct SMSSettings {
		std::string usr;
		std::string pass;
		std::string notifTemplate =
			"Записан час при <ЛДМ>/<ТЕЛ>/ на <ДАТА>, <ДЕН> от <ЧАС>";
		std::string reminderTemplate =
			"Напомняне за час при <ЛДМ>/<ТЕЛ>/ на <ДАТА> от <ЧАС>";
		std::string cancelTemplate =
			"Часът Ви при <ЛДМ> на <ДАТА> от <ЧАС> е отменен";
		int reminder_hours = 6;
		bool hasCredentials() const {
			return !usr.empty() && !pass.empty();
		}
	};

	bool getHisHistoryAuto{ false };
	bool getPisHistoryAuto{ false };
	bool getNraStatusAuto{ false };
	bool getHirbNoAuto{ false };
	bool nhifDailyLimitCheck{ false };
	bool nhifWeekendCheck{ false };
	bool autoStatus{ false };
	bool autoDiagnosis{ true };
	bool showPatientList{ true };
	bool preferMonthlySheets{ false };
	int timeout{ 15 }; //in seconds!
	SMSSettings sms_settings{};
};