#pragma once
#include <string>
#include "Model/Financial/FinancialEnums.h"
#include <vector>
class AbstractReplyHandler;


namespace SOAP
{
	std::string activeHIRBNo(const std::string& id, int personType);
	std::string dentalActivities(const std::string& id, int personType);
	std::string NotifList(const std::string& rziCode);
	std::string getNotificationData(const std::string& rziCode, const std::string& notifHash);
	std::string sendInvoice(const std::string& data, const std::string& rziCode, FinancialDocType purpose);
	std::string sendAmbReport(const std::string& data, const std::string& doctorEgn);
}

namespace PIS
{
	//returns true if the request was passed to the network
	bool sendRequest(const std::string& soapBody, AbstractReplyHandler& handler, int timeout = 10000);
	bool sendBulkRequest(const std::vector<std::string>& soapBodies, AbstractReplyHandler& h);
	
	
}

