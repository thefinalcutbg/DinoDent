#include "Telemetry.h"

#include <JsonCpp/json.h>
#include <QCryptographicHash>

#include "Model/FreeFunctions.h"
#include "Model/User.h"
#include "Version.h"
#include "Network/NetworkManager.h"

#include <qdebug.h>

void Telemetry::sendData()
{
	Json::Value telemetry;

	auto strToHash = User::practice().rziCode + User::doctor().LPK;
	
	telemetry["id"] = QCryptographicHash::hash(
		strToHash.data(),
		QCryptographicHash::Sha256
	)
		.toBase64()
		.toStdString();

	telemetry["datetime"] = FreeFn::getTimeStamp();
	telemetry["version"] = Version::current().toString();
	telemetry["isAdmin"] = User::isAdmin();
	telemetry["nhif"] = User::hasNhifContract();
	telemetry["ekatte"] = User::practice().practice_address.getIdxAsInt();
	telemetry["phone"] = User::doctor().phone;

	NetworkManager::sendTelemetry(Json::FastWriter().write(telemetry));

}


