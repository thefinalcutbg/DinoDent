#include "Telemetry.h"

#include <json/json.h>

#include "Model/User.h"
#include "src/Version.h"
#include "Network/NetworkManager.h"
#include "GlobalSettings.h"
#include "Model/FreeFunctions.h"
#include "Database/DbTelemetry.h"
#include "Database/DbDoctor.h"
#include <QtGlobal>

void Telemetry::sendData()
{
	Json::Value telemetry;

	telemetry["id"] = GlobalSettings::telemetryId();
	telemetry["last_login_date"] = FreeFn::getTimeStampLocal();
	telemetry["version"] = Version::current().toString();
	telemetry["is_admin"] = User::isAdmin();
	telemetry["has_nhif_contract"] = User::hasNhifContract();
	telemetry["ekatte"] = User::practice().practice_address.getIdxAsInt();
	telemetry["phone"] = User::doctor().phone;

	auto dbData = DbTelemetry::getData(User::practice().rziCode, User::doctor().LPK);

	telemetry["amb_count"] = dbData.ambCount;
	telemetry["perio_count"] = dbData.perioCount;
	telemetry["prescr_count"] = dbData.prescrCount;
	telemetry["inv_count"] = dbData.invoiceCount;
	telemetry["patient_count"] = dbData.patientCount;
	telemetry["notice_count"] = dbData.noticeCount;
    telemetry["plan_count"] = dbData.planCount;
	telemetry["has_tablet"] = User::signatureTablet().getHisIdx() > 0;
	telemetry["has_calendar"] = !DbDoctor::calendarRefreshToken(User::doctor().LPK).empty();
	telemetry["has_sms"] = User::settings().sms_settings.hasCredentials();

	int os = -1;

#ifdef Q_OS_WIN
	os = 0;
#endif

#ifdef Q_OS_MACOS
	os = 1;
#endif

#ifdef Q_OS_LINUX
	os = 2;
#endif

	telemetry["os"] = os;

	NetworkManager::sendTelemetry(Json::FastWriter().write(telemetry));
}
