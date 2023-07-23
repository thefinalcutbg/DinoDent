#include "Telemetry.h"

#include <JsonCpp/json.h>

#include "Model/User.h"
#include "Version.h"
#include "Network/NetworkManager.h"
#include "GlobalSettings.h"

void Telemetry::sendData()
{
	Json::Value telemetry;

	telemetry["id"] = GlobalSettings::telemetryId();
	telemetry["version"] = Version::current().toString();
	telemetry["isAdmin"] = User::isAdmin();
	telemetry["hasNhifContract"] = User::hasNhifContract();
	telemetry["ekatte"] = User::practice().practice_address.getIdxAsInt();
	telemetry["phone"] = User::doctor().phone;

	NetworkManager::sendTelemetry(Json::FastWriter().write(telemetry));

}
