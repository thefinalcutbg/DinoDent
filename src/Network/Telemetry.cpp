#include "Telemetry.h"

#include <JsonCpp/json.h>

#include "Model/FreeFunctions.h"
#include "Model/User.h"
#include "Version.h"
#include "Network/NetworkManager.h"

void Telemetry::sendData()
{
	Json::Value telemetry;

	telemetry["datetime"] = FreeFn::getTimeStamp();
	telemetry["version"] = Version::current().toString();
	telemetry["isAdmin"] = User::isAdmin();
	telemetry["nhif"] = User::hasNhifContract();
	telemetry["ekatte"] = User::practice().practice_address.getIdxAsInt();
	telemetry["phone"] = User::doctor().phone;

	NetworkManager::sendTelemetry(Json::FastWriter().write(telemetry));

}


