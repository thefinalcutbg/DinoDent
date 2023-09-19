#include "Telemetry.h"

#include <JsonCpp/json.h>

#include "Model/User.h"
#include "Version.h"
#include "Network/NetworkManager.h"
#include "GlobalSettings.h"
#include "Model/FreeFunctions.h"
#include "Database/DbTelemetry.h"

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

	NetworkManager::sendTelemetry(Json::FastWriter().write(telemetry));

}
