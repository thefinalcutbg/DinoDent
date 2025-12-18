#pragma once
#include <vector>
#include <string>

struct Settings //only practice-specific settings
{
	struct SMSSettings {
		std::string usr;
		std::string pass;
		std::string notifTemplate =
			"Записан час при <ЛДМ> /<ТЕЛ>/ на <ДАТА> от <ЧАС>";
		std::string reminderTemplate =
			"Напомняне за час при <ЛДМ> на <ДАТА> от <ЧАС>";
		std::string cancelTemplate =
			"Часът Ви при <ЛДМ> на <ДАТА> от <ЧАС> е отменен";
		int reminder_hours = 6;
		bool notifAuto = false;
		bool reminderAuto = false;
		bool cancelAuto = false;
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
	bool getHospitalizationAuto{ false };
	bool getAllergiesAuto{ false };
	bool getClinicalConditionsAuto{ false };
	int timeout{ 15 }; //in seconds!
	SMSSettings sms_settings{};
};