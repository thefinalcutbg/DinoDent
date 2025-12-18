#pragma once
#include "Network/AbstractReplyHandler.h"

#include <string_view>
#include <vector>

class ProcedureContainer;
class ToothContainer;
struct Patient;
class Tooth;
class Date;
struct Procedure;

class TiXmlElement;

class HisService : protected AbstractReplyHandler
{
	std::string buildMessage(const std::string& contents, const std::string& patientSig = "");
	std::string signMessage(const std::string& message);

	const std::string messageType;
	const std::string servicePath;

	std::string getQualificationElement(bool includeNhif);

protected:

	bool getBool(TiXmlElement* parent, const std::string& tag);
	std::string getString(TiXmlElement* parent, const std::string& tag);
	int getInt(TiXmlElement* parent, const std::string& tag);
	double getDouble(TiXmlElement* parent, const std::string& tag);
	std::string getFormattedStrName(TiXmlElement* parent, const std::string& tag); //formats str to titlecase
	std::vector<Procedure> getProceduresFromHis(TiXmlElement* doc);
	std::string subject(const Patient& p, bool isPregnant = false, bool isBreastfeeding = false);
	std::string requester(bool includeNhifCode = false);
	std::string performer(bool includeNhifQualification);
	std::string getToothStatus(const Tooth& tooth);
	std::string getMedicalStatus(const Patient& p);
	std::string getProcedure(const Procedure& p, const ToothContainer& teeth, ToothContainer& teethChanged, int sequence);
	std::string bind(const std::string& name, const std::string& value, bool isUserInput = false);
	std::string bind(const std::string& name, int value, bool ommitZero = true);
	std::string bind(const std::string& name, bool value);
	std::string bind(const std::string& name, double value);
	std::string bind(const std::string& name, const char* value, bool isUserInput = false);
	std::string openTag(const std::string& tag);
	std::string closeTag(const std::string tag);
	//returns the xml block and the signature bitmap
	std::pair<std::string, std::vector<unsigned char>> generatePatientSignature(const std::string& contents, const Patient& patient, bool patientIsSigner);
//	std::string initialStatusAsProcedure(const ToothContainer& teeth, const Date& lastProcedureDate, bool augmentation = false);

	std::string getErrors(const std::string& reply);

	HisService(const std::string& messageType, const std::string& servPath)
		:
		messageType{ messageType },
		servicePath{ servPath }
	{}
public:
	static inline std::string hisUrl = "https://api.his.bg";
	void abortRequest();
	bool sendRequestToHis(const std::string& contents, const std::string& patientSignature = "");
	bool sendRequestToHisNoAuth(const std::string& contents);

};

