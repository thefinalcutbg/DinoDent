#include "Telemetry.h"

#include <JsonCpp/json.h>

#include "Model/User.h"
#include "Version.h"
#include "Network/NetworkManager.h"
#include "GlobalSettings.h"
#include "Model/FreeFunctions.h"

void Telemetry::sendData()
{
	Json::Value telemetry;

	telemetry["id"] = GlobalSettings::telemetryId();
	telemetry["last_login_date"] = FreeFn::getTimeStamp();
	telemetry["version"] = Version::current().toString();
	telemetry["is_admin"] = User::isAdmin();
	telemetry["has_nhif_contract"] = User::hasNhifContract();
	telemetry["ekatte"] = User::practice().practice_address.getIdxAsInt();
	telemetry["phone"] = User::doctor().phone;

	NetworkManager::sendTelemetry(Json::FastWriter().write(telemetry));

}
