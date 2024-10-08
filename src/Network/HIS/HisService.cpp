﻿#include <TinyXML/tinyxml.h>
#include <QDateTime>

#include "HisService.h"
#include "Network/signer.h"
#include "Network/PKCS11.h"
#include "Network/NetworkManager.h"
#include "HisToken.h"
#include "View/ModalDialogBuilder.h"
#include "Model/User.h"
#include "Model/FreeFunctions.h"
#include "Model/Patient.h"
#include "Model/Dental/AmbList.h"
#include "Model/FreeFunctions.h"
#include "Model/Dental/ToothUtils.h"

bool HisService::sendRequestToHis(const std::string& query)
{
	if (awaiting_reply) return false;

	if (HisToken::getToken().empty()) {
		return HisToken::requestToken(this, query, !show_dialogs);
	}

	auto signedMsg = signMessage(buildMessage(query));
//	ModalDialogBuilder::showMultilineDialog(signedMsg);
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
	PKCS11 hsm;

	if (!hsm.hsmLoaded())
	{
		ModalDialogBuilder::showMessage("Не е открит КЕП");
		return {};
	}

	if (hsm.loginRequired()) {

		NetworkManager::clearAccessCache();
		auto pin = ModalDialogBuilder::pinPromptDialog(hsm.pem_x509cert(), hsm.driver);

		if (pin.empty()) {
			return {};
		}


		if (!hsm.login(pin))
		{
			ModalDialogBuilder::showError("Грешна парола или блокирана карта");
			return {};
		};
	}

	return Signer::signEnveloped(message, hsm.takePrivateKey(), hsm.x509ptr(), true);
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
				"<nhis:createdOn  value=\"" + FreeFn::getTimeStampUTC() + "\"/>"
			"</nhis:header>"
		
			"<nhis:contents>"
				+query+
			"</nhis:contents>"
	"</nhis:message>"
	
	;

	return result;
	
}

std::string HisService::subject(const Patient& p, bool isPregnant, bool isBreastfeeding)
{
	std::string middleNameTag = p.MiddleName.size()?
		"<nhis:middle value=\"" + p.MiddleName + "\"/>" : "";

	std::string preg_breastfeed;

	if (p.canBePregnant())
	{
		preg_breastfeed += bind("isPregnant", isPregnant);
		preg_breastfeed += bind("isBreastFeeding", isBreastfeeding);
	}

	int patientType = p.type == Patient::EU ? 5 : p.type;

	std::string subject =
	"<nhis:subject>"
		+ bind("identifierType", patientType)
		+ bind("identifier", p.id)
		+ bind("nhifInsuranceNumber", p.HIRBNo)
		+ bind("birthDate", p.birth.to8601())
		+ bind("gender", p.sex + 1)
		+"<nhis:name>"
			+ bind("given", p.FirstName, true)
			+ bind("middle", p.MiddleName, true)
			+ bind("family", p.LastName, true)
		+"</nhis:name>"
		 "<nhis:address>"
			+ bind("country", p.foreigner ? p.foreigner->country.getCode() : "BG" )
			+ bind("county", p.foreigner ? "" : p.city.getRegionCode())
			+ bind("ekatte", p.foreigner ? "" : p.city.ekatte())
			+ bind("city", p.foreigner ? p.foreigner->city : p.city.getString(), true)
			+ bind("line", p.foreigner ? "" : FreeFn::escapeXml(p.getFullAddress()))
		+"</nhis:address>"
		+bind("phone", p.phone)
		//<nhis:email value="[string]"/>
		+"<nhis:various>"
			+bind("age", p.getAge())
			+preg_breastfeed
		+ "</nhis:various>"
	"</nhis:subject>"
	;

	return subject;
}


std::string HisService::requester(bool nhif)
{

	std::string nhifCode = User::practice().nhif_contract && nhif ?
		" nhifCode=\"" + std::to_string(User::doctor().specialtyAsInt()) + "\""
		:
		"";

	std::string qualification =
		"<nhis:qualification value=\"" + std::to_string(User::doctor().hisSpecialty.getIdx()) + "\"" + nhifCode + "/>";


	std::string rhifAreaNumber;
	
	if (nhif) {
		rhifAreaNumber = bind("rhifAreaNumber", 
			User::practice().practice_address.getRhif() + 
			User::practice().practice_address.getHealthRegion());
	}


	std::string requester =
		"<nhis:requester>"
			"<nhis:pmi value=\"" + User::doctor().LPK + "\"/>"
			+ qualification +
			"<nhis:role value=\"1\"/>"
			"<nhis:practiceNumber value=\"" + User::practice().rziCode + "\"/>"
			+ rhifAreaNumber + 
			"<nhis:nhifNumber value=\"" + User::practice().practice_address.getRhif() + "\"/>"
			"<nhis:phone value=\"" + User::doctor().phone + "\"/>"
		"</nhis:requester>"
		;
	return requester;

}

std::string HisService::performer()
{

	std::string nhifCode = User::hasNhifContract() ?
		" nhifCode=\"" + std::to_string(User::doctor().specialtyAsInt()) + "\""
		:
		"";

	std::string qualification =
		"<nhis:qualification value=\"" + std::to_string(User::doctor().hisSpecialty.getIdx()) + "\"" + nhifCode + "/>";

	std::string performer;
	performer += "<nhis:performer>";
	performer += bind("pmi", User::doctor().LPK);
	performer += qualification;
	performer += bind("role", 1);
	performer += bind("practiceNumber", User::practice().rziCode);
	performer += bind("nhifNumber", User::practice().practice_address.getRhif());
	performer += bind("phone", User::doctor().phone);
	performer += "</nhis:performer>";
		
	return performer;
}

std::string HisService::getToothStatus(const Tooth& tooth)
{
	std::string result;

	std::vector<std::string> statuses = tooth.getHISStatus();

	if (statuses.empty()) return result;

	result += "<nhis:tooth>";
	result += bind("toothIndex", ToothUtils::getToothNumber(tooth.index(), tooth[Dental::Temporary]));
	
	if (tooth.isSupernumeral()) {
		result += bind("supernumeralIndex", 1);
	}

	for (auto& s : statuses)
	{
		result += "<nhis:condition>";

		result += bind("code", s);

		result += "</nhis:condition>";
	}

	result += "</nhis:tooth>";

	return result;
}

std::string HisService::getMedicalStatus(const Patient& p)
{
	std::string result;

	result += openTag("medicalStatus");

	for (auto& allergy : p.allergies)
	{
		if (allergy.nrn.empty()) continue;

		result += bind("nrnAllergy", allergy.nrn);
	}

	result += closeTag("medicalStatus");

	return result;
}

std::string HisService::getProcedure(const Procedure& p, const ToothContainer& teeth, ToothContainer& teethChanged, int sequence)
{
	if (!p.isHisSupported()) return std::string();

	std::string result;
	result.reserve(200);

	result += "<nhis:dentalProcedure>";

	result += bind("sequence", sequence);

	if (p.his_index) {
		result += bind("index", p.his_index);
	}

	result += bind("code", p.code.code());
	result += bind("type", p.code.hisType());

	if (p.code.type() == ProcedureType::anesthesia)
	{
		result += bind("duration", std::get<AnesthesiaMinutes>(p.result).minutes);
	}

	result += bind("datePerformed", p.date.to8601());

	result += bind("financingSource", static_cast<int>(p.financingSource));

	if (p.code.isToothSpecific())
	{

		p.applyProcedure(teethChanged);

		result += getToothStatus(teethChanged.at(p.tooth_idx));
	
	}

	if (p.code.isRangeSpecific())
	{
		auto [begin, end] = std::get<ConstructionRange>(p.result);

		p.applyProcedure(teethChanged);

		for (int i = begin; i <= end; i++)
		{
			if (p.code.type() == ProcedureType::denture && 
				!teethChanged.at(i)[Dental::Denture]){ 
				continue; 
			}

			result += getToothStatus(teethChanged.at(i));
		}
	}

	if (p.code.type() == ProcedureType::depuratio)
	{
		p.applyProcedure(teethChanged);

		for (int i = 0; i < Dental::teethCount; i++)
		{
			if (teeth[i][Dental::Calculus]) 
				result += getToothStatus(teethChanged[i]);

			if (teeth[i].getSupernumeral()[Dental::Calculus])
				result += getToothStatus(teethChanged[i].getSupernumeral());
		}
	}

	if (p.code.type() == ProcedureType::full_exam)
	{
		
		for (auto& tooth : teeth)
		{
			result += getToothStatus(tooth);

			if (tooth[Dental::HasSupernumeral]) {
				result += getToothStatus(tooth.getSupernumeral());
			}
		}

	}



	result += bind("note", p.notes, true);

	if (p.diagnosis.index() != 0) {

		result += "<nhis:diagnosis>";
		result += bind("code", p.diagnosis.index());
		result += bind("note", p.diagnosis.description, true);
		result += "</nhis:diagnosis>";

	}

	result += "</nhis:dentalProcedure>";

	return result;
}

std::string HisService::bind(const std::string& name, double value)
{
	return value ? bind(name, FreeFn::formatDouble(value)) : "";
}

std::string HisService::bind(const std::string& name, const char* value, bool isUserInput)
{
    if (value == nullptr || std::string(value).empty()) return "";

    return bind(name, std::string{ value }, isUserInput);
}

std::string HisService::openTag(const std::string& tag)
{
	return "<nhis:" + tag + ">";
}

std::string HisService::closeTag(const std::string tag)
{
	return "</nhis:" + tag + ">";
}
/*
std::string HisService::initialStatusAsProcedure(const ToothContainer& teeth, const Date& lastProcedureDate, bool augmentation)
{

	std::string result;

	result += "<nhis:dentalProcedure>";

	result += bind("sequence", 1);

	if (augmentation) {
		result += bind("index", 1);
	}

	result += bind("code", "D-01-001");
	result += bind("type", 1);
	result += bind("datePerformed",lastProcedureDate.to8601());
	result += bind("financingSource", 4);

	for (auto& tooth : teeth)
	{
		result += getToothStatus(tooth);

		if (tooth[Dental::HasSupernumeral]) result += getToothStatus(tooth.getSupernumeral());
	}
	result += bind("note", "ИЗХОДЯЩ СТАТУС (автоматично генерирана)");
	result += "</nhis:dentalProcedure>";

	return result;
}
*/
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

bool HisService::getBool(TiXmlElement* parent, const std::string& tag)
{
	auto element = parent->FirstChildElement(std::string{"nhis:"} + tag);

	if (!element || !element->FirstAttribute()) return false;

	return element->FirstAttribute()->ValueStr() == "true";
}

std::string HisService::getString(TiXmlElement* parent, const std::string& tag)
{
	auto element = parent->FirstChildElement(std::string{"nhis:"} + tag);

	if (!element || !element->FirstAttribute()) return "";

	return element->FirstAttribute()->ValueStr();
}

int HisService::getInt(TiXmlElement* parent, const std::string& tag)
{
	auto element = parent->FirstChildElement(std::string{"nhis:"} + tag);

	if (!element || !element->FirstAttribute()) return 0;

	return element->FirstAttribute()->IntValue();
}

double HisService::getDouble(TiXmlElement* parent, const std::string& tag)
{
	auto element = parent->FirstChildElement(std::string{"nhis:"} + tag);

	if (!element || !element->FirstAttribute()) return 0.0;

	return element->FirstAttribute()->DoubleValue();
}
