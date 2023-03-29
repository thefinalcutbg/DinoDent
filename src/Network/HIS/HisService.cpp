#include <TinyXML/tinyxml.h>
#include <QDateTime>

#include "HisService.h"
#include "Network/XmlSigner.h"
#include "Network/PKCS11.h"
#include "Network/NetworkManager.h"
#include "HisToken.h"
#include "View/ModalDialogBuilder.h"
#include "Model/User.h"
#include "Model/FreeFunctions.h"
#include "Model/Patient.h"
#include "Model/Dental/AmbList.h"
#include "Model/FreeFunctions.h"

std::string timeNow() {
	auto t = QDateTime::currentDateTime();
	return t.toString(Qt::DateFormat::ISODate).toStdString();
}

bool HisService::sendRequestToHis(const std::string& query)
{
	if (awaiting_reply) return false;
	//ModalDialogBuilder::showMultilineDialog(buildMessage(query));// return true;

	if (HisToken::getToken().empty()) {
		return HisToken::requestToken(this, query);
	}

	auto signedMsg = signMessage(buildMessage(query));

	if (signedMsg.empty()) return false;

	awaiting_reply = true;

	NetworkManager::sendRequestToHis(
		  this, 
		  signedMsg, 
		  HisToken::getToken(), 
		  hisUrl + servicePath
	);

	return true;
	
}

bool HisService::sendRequestToHisNoAuth(const std::string& query)
{
	NetworkManager::sendRequestToHisNoAuth(this, buildMessage(query), hisUrl + servicePath);
	return true;
}

const std::string HisService::signMessage(const std::string& message)
{
	PKCS11 signer;

	if (!signer.hsmLoaded())
	{
		ModalDialogBuilder::showMessage("Не е открит КЕП");
		return {};
	}

	if (signer.loginRequired()) {

		NetworkManager::clearAccessCache();
		auto pin = ModalDialogBuilder::pinPromptDialog(signer.pem_x509cert());

		if (pin.empty()) {
			return {};
		}


		if (!signer.login(pin))
		{
			ModalDialogBuilder::showError("Грешна парола или блокирана карта");
			return {};
		};
	}

	return XmlSigner::signNhifMessage(message, signer.takePrivateKey(), signer.pem_x509cert());
}

const std::string HisService::buildMessage(const std::string& query)
{

	constexpr const char* softwareName = "DinoDent";

	std::string result;

	result.reserve(4000);

	result +=

	"<nhis:message "
		"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
		"xmlns:nhis=\"https://www.his.bg\" "
		"xsi:schemaLocation=\"https://www.his.bg/api/v1/NHIS-" + messageType + ".xsd\">"
	
			"<nhis:header>"
				"<nhis:sender value=\"2\"/>"
				"<nhis:senderId value=\"" + User::doctor().LPK + "\"/>"
				"<nhis:senderISName value=\"" + softwareName + "\"/>"
				"<nhis:recipient value=\"4\"/>"
				"<nhis:recipientId value=\"NHIS\"/>"
				"<nhis:messageId value=\"" + FreeFn::getUuid() + "\"/>"
				"<nhis:messageType value=\"" + messageType + "\"/>"
				"<nhis:createdOn  value=\"" + timeNow() + "\"/>"
			"</nhis:header>"
		
			"<nhis:contents>"
				+query+
			"</nhis:contents>"
	"</nhis:message>"
	
	;


	ModalDialogBuilder::showMultilineDialog(result);


	return result;
	
}

std::string HisService::subject(const Patient& p)
{
	std::string middleNameTag = p.MiddleName.size()?
		"<nhis:middle value=\"" + p.MiddleName + "\"/>" : "";

	std::string subject =
	"<nhis:subject>"
		+ bind("identifierType", std::to_string(p.type))
		+ bind("identifier", p.id)
		+ bind("nhifInsuranceNumber", p.HIRBNo)
		+ bind("birthDate", p.birth.to8601())
		+ bind("gender", p.sex + 1)
		+"<nhis:name>"
			+ bind("given", p.FirstName)
			+ bind("middle", p.MiddleName)
			+ bind("family", p.LastName)
		+"</nhis:name>"
		 "<nhis:address>"
			+ bind("country", "BG")
			+ bind("county", p.city.getRegionCode())
			+ bind("ekatte", p.city.ekatte())
			+ bind("city", p.city.getString())
			+ bind("line", FreeFn::escapeXml(p.getFullAddress()))
		+"</nhis:address>"
		+bind("phone", p.phone)
		//<nhis:email value="[string]"/>
		+"<nhis:various>"
			+bind("age", p.getAge())
		+ "</nhis:various>"
	"</nhis:subject>"
	;

	return subject;
}


std::string HisService::requester(bool nhif)
{

	std::string nhifCode = User::practice().nzok_contract && nhif ?
		" nhifCode=\"" + std::to_string(User::doctor().specialtyAsInt()) + "\""
		:
		"";

	std::string qualification =
		"<nhis:qualification value=\"" + std::to_string(User::doctor().hisSpecialty.getIdx()) + "\"" + nhifCode + "/>";

	std::string requester =
		"<nhis:requester>"
			"<nhis:pmi value=\"" + User::doctor().LPK + "\"/>"
			+ qualification +
			"<nhis:practiceNumber value=\"" + User::practice().rziCode + "\"/>"
			"<nhis:nhifNumber value=\"" + User::practice().RHIF() + "\"/>"
			"<nhis:phone value=\"" + User::doctor().phone + "\"/>"
		"</nhis:requester>"
		;
	return requester;

}

std::string HisService::performer()
{

	std::string nhifCode = User::hasNzokContract() ?
		" nhifCode=\"" + std::to_string(User::doctor().specialtyAsInt()) + "\""
		:
		"";

	std::string qualification =
		"<nhis:qualification value=\"" 
			+ std::to_string(User::doctor().hisSpecialty.getIdx())
			+ "\"" + nhifCode + "/>";


	std::string performer;
	performer += "<nhis:performer>";
	performer += bind("pmi", User::doctor().LPK);
	performer += qualification;
	performer += bind("role", 1);
	performer += bind("practiceNumber", User::practice().rziCode);
	performer += bind("nhifNumber", User::practice().RHIF());
	performer += bind("phone", User::doctor().phone);
	performer += "</nhis:performer>";
		
	return performer;
}

std::string HisService::getStatus(const ToothContainer& teeth)
{
	std::string result;

	result.reserve(1000);

	result += "<nhis:dentalStatus>";

	for (auto& tooth : teeth)
	{
		result += getToothStatus(tooth);
	}

	result += "</nhis:dentalStatus>";

	return result;
}

std::string HisService::getProcedures(const ProcedureContainer& procedures, const ToothContainer& teeth)
{
	std::string result;

	result.reserve(1000);

	int sequence = 0;

	ToothContainer teethChanged = teeth;

	for (auto& p : procedures)
	{
		sequence++;

		result += "<nhis:dentalProcedure>";


		result += bind("sequence", sequence);

		result += bind("code", p.code.code());
		result += bind("type", static_cast<int>(p.code.type()));


		result += bind("datePerformed", p.date.to8601());

		result += bind("financingSource", static_cast<int>(p.financingSource));
	
		if (p.isToothSpecific())
		{
			p.applyProcedure(teethChanged);

			result += getToothStatus(teethChanged.at(p.tooth));
		}

		if (p.isRangeSpecific())
		{
			auto [begin, end] = std::get<ConstructionRange>(p.result);
			//result += bind("fromToothIndex", ToothUtils::getToothNumber(begin, false));
			//result += bind("toToothIndex", ToothUtils::getToothNumber(end, false));

			p.applyProcedure(teethChanged);

			for (int i = begin; i <= end; i++)
			{
				result += getToothStatus(teethChanged.at(i));
			}
		}


		result += bind("note", p.notes);

		result += "<nhis:diagnosis>";
		result += bind("note", p.diagnosis);
		result += "</nhis:diagnosis>";

		result += "</nhis:dentalProcedure>";

	}

	return result;
}

std::string HisService::getToothStatus(const Tooth& tooth)
{
	std::string result;

	std::vector<int> statuses;

	auto boolStatus = tooth.getBoolStatus();

	for (int i = 0; i < statusCount; i++) {
		if (boolStatus[i]) statuses.push_back(StatusCode::hisNum[i]);
	}

	if (statuses.empty()) return result;

	result += "<nhis:tooth>";
	result += bind("toothIndex", ToothUtils::getToothNumber(tooth.index, tooth.temporary));

	for (auto& s : statuses)
	{
		result += "<nhis:condition>";

		result += bind("code", s);

		switch (s)
		{
		case 11: //mobility
		{
			result += bind("mobilityDegree",
				static_cast<int>(tooth.mobility.degree) + 1
			);
		}
		break;

		case 6: //restoration
		{
			for (int i = 0; i < surfaceCount; i++)
				if (tooth.obturation.exists(i))
					result += bind("surface", i + 1);
		}
		break;

		case 2: //caries
		{
			for (int i = 0; i < surfaceCount; i++)
				if (tooth.caries.exists(i))
					result += bind("surface", i + 1);
		}
		break;

		case 17: //bridge
		{
			result += bind(
				"isEndmost",
				tooth.bridge.position != BridgePos::Middle ? "true" : "false"
			);
		}
		break;

		case 18: //splint
		{
			result += bind(
				"isEndmost",
				tooth.splint.position != BridgePos::Middle ? "true" : "false"
			);
		}
		break;
		}

		result += "</nhis:condition>";
	}

	result += "</nhis:tooth>";

	return result;
}

std::string HisService::bind(const std::string& name, double value)
{
	return value ? bind(name, FreeFn::formatDouble(value)) : "";
}

std::string HisService::bind(const std::string& name, const char* value, bool isUserInput)
{
	if (value == "") return "";

	return bind(name, std::string{ value }, isUserInput);
}

std::string HisService::bind(const std::string& name, const std::string& value, bool isUserInput)
{
	if (value.empty()) return "";

	auto result = "<nhis:" + name + " value=\"";

	result += isUserInput ? FreeFn::escapeXml(value) : value;

	result += "\" />";

	return result;
}

std::string HisService::bind(const std::string& name, int value, bool ommitZero)
{
	if (!value && ommitZero) return {};

	return bind(name, std::to_string(value));
}

std::string HisService::bind(const std::string& name, bool value)
{
	return bind(name, value ? "true" : "false");
}

#include <TinyXML/tinyxml.h>

std::string HisService::getErrors(const std::string& reply)
{
	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto contentChild = docHandle.
			FirstChild(). //message
			Child(1).	  //contents
			FirstChildElement(). //possible error
			ToElement();

	while (contentChild)
	{
		if (contentChild->ValueStr() == "nhis:error") {

			return
				contentChild->
					FirstChild()->
						NextSiblingElement()->
								Attribute("value");
		}

		contentChild = contentChild->NextSiblingElement();
	}

	return std::string{};

}