#pragma once
#include <string>
#include <vector>
#include "Model/Date.h"
class AbstractReplyHandler;

enum class FilePurpose{Invoice, DebitNote, CreditNote, AmbReport};

namespace SOAP
{
	std::string activeHIRBNo(const std::string& id, int personType);
	std::string dentalActivities(const std::string& id, int personType);
	std::string NotifList(const std::string& doctorEGN);
	std::string getNotificationData(const std::string& doctorEGN, const std::string& notifHash);
	std::string sendFile(const std::string& file, const std::string& doctorEGN, FilePurpose purpose);
}

namespace PIS
{
	//returns true if the request was passed to the network
	bool sendRequest(const std::string& soapBody, AbstractReplyHandler& handler);
	
}

