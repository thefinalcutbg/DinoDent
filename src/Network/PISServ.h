#pragma once
#include <string>
#include "Model/Financial/FinancialEnums.h"
#include "Model/Date.h"
#include <vector>

class AbstractReplyHandler;

struct Patient;

namespace PISQuery
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
	enum class SOAPAction { View, Files };
	//returns true if the request was passed to the network
	bool sendRequest(const std::string& soapBody, AbstractReplyHandler& handler, SOAPAction header = SOAPAction::View);
	bool insuranceRequest(AbstractReplyHandler& handler, const Patient& p, const Date& date = Date::currentDate());
}


