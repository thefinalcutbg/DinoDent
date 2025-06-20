#include "EDental.h"

#include <array>
#include <map>
#include <unordered_map>
#include <algorithm>

#include "Model/User.h"
#include <TinyXML/tinyxml.h>
#include "View/ModalDialogBuilder.h"
#include "Model/Dental/ToothUtils.h"
#include "Model/Dental/ToothContainer.h"
#include "HISHistoryAlgorithms.h"
#include "Model/FreeFunctions.h"


bool EDental::Open::sendRequest(
    const AmbList& ambSheet, 
    const Patient& patient, 
	decltype(m_callback) nrnCallback
)
{
	m_callback = nrnCallback;

	std::string contents;

	bool isNhif = ambSheet.isNhifSheet();

	bool adverseConditions = isNhif && ambSheet.nhifData.isUnfavourable && User::practice().isUnfavourable();

	contents.reserve(4000);

	contents +=
		"<nhis:dentalTreatment>"
		+ bind("lrn", ambSheet.lrn)
		//+ bind("basedOn", ambSheet.basedOn) //needs closing functionality
			+ bind("treatmentStart", FreeFn::LocalToUTC(ambSheet.date))
			//+ bind("treatmentEnd", ambSheet.time.to8601(ambSheet.getDate())) //TO IMPLEMENT!!!
			+ bind("adverseConditions", adverseConditions)
			+ bind("rhifAreaNumber", patient.city.getRhif() + patient.city.getHealthRegion())
			+ HisService::getMedicalStatus(patient)
			+ getProcedures(ambSheet.procedures, ambSheet.teeth, ambSheet.date)
		+"</nhis:dentalTreatment>"
		+ HisService::subject(patient)
		+ HisService::performer(isNhif)
	;		

	std::string patientSignature;

	if (User::signatureTablet().getHisIdx())
	{
		patientSignature = HisService::generatePatientSignature(contents, patient);
	}

	return HisService::sendRequestToHis(contents, patientSignature);
}

std::string EDental::Open::getProcedures(const ProcedureContainer& procedures, const ToothContainer& teeth, const Date& treatmentStartDate)
{

	std::string result;

	result.reserve(1000);

	ToothContainer teethChanged = teeth;

	int sequence = 0;

	for (auto& p : procedures)
	{
		sequence++;
		result += HisService::getProcedure(p, teeth, teethChanged, sequence);
	}

	return result;

}

void EDental::Open::parseReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback = nullptr;
		return;
	}

	auto errors = getErrors(reply);

	if (errors.size()) {
		
		if (FreeFn::contains(errors, "Вече има подаден")) {
			auto existingNrn = errors.substr(errors.size() - 12);
			m_callback(existingNrn, {}, true);
			return;
		}

		ModalDialogBuilder::showError(errors);
		m_callback = nullptr;
		return;


	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto nrn = docHandle.
		FirstChild(). //message
		Child(1).	  //contents
		FirstChildElement().ToElement(); //nrn

	std::string nrnStr;

	if (nrn) {
		nrnStr = nrn->Attribute("value");
	}

	std::vector<std::pair<int, int>> seqIndexPair;

	auto contents = docHandle.
		FirstChild(). //message
		Child(1);	  //contents


	//dentalProcedures
	for (int i = 3; contents.Child(i).ToElement(); i++)
	{
		auto index = std::stoi(contents.Child(i).Child(1).ToElement()->Attribute("value"));
		auto sequence = std::stoi(contents.Child(i).Child(0).ToElement()->Attribute("value"));

		seqIndexPair.push_back(std::make_pair(sequence-1, index));
	}

	m_callback(nrnStr, seqIndexPair, false);

}

bool EDental::Augment::sendRequest(const AmbList& ambSheet, const Patient& patient, bool removeAutoStatus, decltype(m_callback) callback)
{
	m_callback = callback;

	std::string contents;

	bool isNhif = ambSheet.isNhifSheet();

	bool adverseConditions = isNhif && ambSheet.nhifData.isUnfavourable && User::practice().isUnfavourable();

	contents.reserve(4000);

	std::string resultingDocuments = "<nhis:documents>";

	for(auto& ref : ambSheet.referrals) resultingDocuments += bind("nrnReferral", ref.nrn);

	resultingDocuments += bind("issuedMedicalNotice", !ambSheet.medical_notices.empty());
	
	//implement prescriptions here

	resultingDocuments += "</nhis:documents>";

	contents +=
		"<nhis:dentalTreatment>"
		+ bind("nrnDental", ambSheet.nrn)
		//+ bind("basedOn", ambSheet.basedOn)
		+ bind("treatmentStart", FreeFn::LocalToUTC(ambSheet.date))
		+ bind("adverseConditions", adverseConditions)
		+ bind("rhifAreaNumber", patient.city.getRhif() + patient.city.getHealthRegion())
		+ HisService::getMedicalStatus(patient)
		+ resultingDocuments
		+ getProcedures(ambSheet.procedures, ambSheet.teeth, ambSheet.date, removeAutoStatus)
		+ "</nhis:dentalTreatment>"
		+ HisService::subject(patient)
		+ HisService::performer(isNhif)
		;

	std::string patientSignature;

	if (User::signatureTablet().getHisIdx())
	{
		patientSignature = HisService::generatePatientSignature(contents, patient);
	}

	return HisService::sendRequestToHis(contents, patientSignature);
}

std::string EDental::Augment::getProcedures(const ProcedureContainer& procedures, const ToothContainer& teeth, const Date& treatmentStartDate, bool autoStatusRemove)
{
	
	std::string result;

	result.reserve(1000);

	int sequence = 0;

	ToothContainer teethChanged = teeth;

	for (auto& p : procedures)
	{
		sequence++;

		result += HisService::getProcedure(p, teeth, teethChanged, sequence);

	}

	sequence = 999;

	for (auto& p : procedures.removedProcedures()) {

		sequence++;

		result += "<nhis:dentalProcedure>";
		result += bind("sequence", sequence);
		result += bind("index", p.his_index);
		result += bind("code", p.code.code());
		result += bind("status", 7);
		result += bind("type", p.code.achiBlock());
		result += bind("datePerformed", p.date.to8601());
		result += bind("financingSource", static_cast<int>(p.financingSource));
		result += "</nhis:dentalProcedure>";

	}

	if (autoStatusRemove) {

		result += "<nhis:dentalProcedure>";
		result += bind("sequence", 9999);
		result += bind("index", 1);
		result += bind("code", "D-01-001");
		result += bind("type", 1);
		result += bind("status", 7);
		result += bind("datePerformed", treatmentStartDate.to8601());
		result += bind("financingSource", 4);
		result += "</nhis:dentalProcedure>";
	}
	
	return result;
	
}

void EDental::Augment::parseReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback = nullptr;
		return;
	}

	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		m_callback = nullptr;
		return;
	}


	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto contents = docHandle.
		FirstChild(). //message
		Child(1);	  //contents

	std::vector<std::pair<int, int>> seqIdxPair;

	//dentalProcedures
	for (int i = 2; contents.Child(i).ToElement(); i++)
	{
		//sequence
		auto sequence = std::stoi(contents.Child(i).Child(0).ToElement()->Attribute("value"));
		//index
		auto hisIdx = std::stoi(contents.Child(i).Child(1).ToElement()->Attribute("value"));
		
		if (sequence >= 999) continue;

		seqIdxPair.push_back(std::make_pair(sequence - 1, hisIdx));
	}

	m_callback(seqIdxPair);
}

bool EDental::Cancel::sendRequest(const std::string& nrn, std::function<void(bool)> success)
{
	m_callback = success;

	auto reason = ModalDialogBuilder::inputDialog(
		"Основание за анулиране на денталния преглед:",
		"Анулиране на дентален преглед"
	);

	if (reason.empty()) return false;

	std::string contents =
		bind("nrnDental", nrn)
		+ bind("cancelReason", reason);

	return HisService::sendRequestToHis(contents);
}

void EDental::Cancel::parseReply(const std::string& reply)
{
	auto errors = getErrors(reply);

	if (errors.size()) {

		ModalDialogBuilder::showError(errors);

		if (errors == "Прегледът вече е анулиран") {
			m_callback(true);
		}

		m_callback = nullptr;
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto status = docHandle.
		FirstChild(). //message
		Child(1).	  //contents
		Child(1).ToElement(); //status

	if (
		status &&
		status->FirstAttribute()->IntValue() == 3
		)
	{
		m_callback(true); 
		m_callback = nullptr;
		return;
	}

	m_callback(false);
	m_callback = nullptr;
}


bool EDental::GetStatus::sendRequest(const Patient& patient, std::function<void(const ToothContainer&)> callback)
{
	m_callback = callback;

	std::string contents =
		bind("identifierType", patient.type) +
		bind("identifier", patient.id) +
		bind("fromDate", Date().to8601()) +
		bind("toDate", Date::currentDate().to8601()) +
		bind("practiceNumber", User::practice().rziCode)
		;

	return HisService::sendRequestToHis(contents);
}

void EDental::GetStatus::parseReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback = nullptr;
		return;
	}

	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		m_callback = nullptr;
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	auto status_result = HISHistoryAlgorithms::getDentalHistory(doc);

	if (status_result.empty()) status_result.emplace_back();

	m_callback(status_result.back().teeth);

	m_callback = nullptr;
	

}

void EDental::GetProcedures::parseReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback = nullptr;
		return;
	}

	auto errors = getErrors(reply);

	if (errors.size()) {
		if (show_dialogs) {
			ModalDialogBuilder::showError(errors);
		}
		m_callback = nullptr;
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	m_callback(HISHistoryAlgorithms::getProcedures(doc));

	m_callback = nullptr;

}

bool EDental::GetProcedures::sendRequest(const Patient& patient, bool showDialogs, decltype(m_callback) callback)
{

	this->show_dialogs = showDialogs;

	m_callback = callback;

	std::string contents =
		bind("identifierType", patient.type) +
		bind("identifier", patient.id) +
		bind("fromDate", "2008-01-01") +
		bind("toDate", Date::currentDate().to8601()) +
		bind("practiceNumber", User::practice().rziCode)
		;

	return HisService::sendRequestToHis(contents);
}

#include "HISHistoryAlgorithms.h"

void EDental::GetStatusAndProcedures::parseReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback = nullptr;
		return;
	}

	auto errors = getErrors(reply);

	if (errors.size()) {
		if (show_dialogs) {
			ModalDialogBuilder::showError(errors);
		}
		m_callback = nullptr;
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

    m_callback(HISHistoryAlgorithms::getProcedures(doc), HISHistoryAlgorithms::getDentalHistory(doc));

	m_callback = nullptr;
}

bool EDental::GetStatusAndProcedures::sendRequest(const Patient& patient,bool showDialogs, decltype(m_callback) callback)
{
	this->show_dialogs = showDialogs;

	m_callback = callback;

	std::string contents =
		bind("identifierType", patient.type) +
		bind("identifier", patient.id) +
		bind("fromDate", "2008-01-01") +
		bind("toDate", Date::currentDate().to8601()) +
		bind("practiceNumber", User::practice().rziCode)
		;

	return HisService::sendRequestToHis(contents);
}


bool EDental::GetDentalHistory::sendRequest(const Patient& patient, decltype(m_callback) callback)
{
	m_callback = callback;

	std::string contents =
		bind("identifierType", patient.type) +
		bind("identifier", patient.id) +
		bind("fromDate", Date().to8601()) +
		bind("toDate", Date::currentDate().to8601()) +
		bind("practiceNumber", User::practice().rziCode)
		;

	return HisService::sendRequestToHis(contents);
}

void EDental::GetDentalHistory::parseReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback = nullptr;
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	m_callback(HISHistoryAlgorithms::getDentalHistory(doc));

	m_callback = nullptr;
}

bool EDental::Fetch::sendRequest(const std::string& nrn, decltype(m_callback) callback)
{
	m_callback = callback;

	std::string contents;

	contents += bind("nrnDental", nrn);

	return HisService::sendRequestToHis(contents);
}

void EDental::Fetch::parseReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback = nullptr;
		return;
	}


	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		m_callback = nullptr;
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto ambXml = docHandle.
		FirstChild().				//message
		Child(1).					//contents
		Child(1).					//results
		FirstChild().ToElement();	//dentalTreatment

	if (!ambXml) {
		m_callback = nullptr;
		return;
	}

	AmbList list;
	Patient patient;

	//basic properties
	list.nrn = getString(ambXml, "nrnDental");
	list.lrn = getString(ambXml, "lrn");
	list.date = FreeFn::UTCToLocal(getString(ambXml, "treatmentStart"));
	list.nhifData.isUnfavourable = getBool(ambXml, "adverseConditions");
	list.his_updated = true;

	//parsing the procedures (brace yourselves)
	for (
		auto procXml = ambXml->FirstChildElement("nhis:dentalProcedure");
		procXml != nullptr;
		procXml = procXml->NextSiblingElement("nhis:dentalProcedure")
		)
	{

		//entered in error
		if (getInt(procXml, "status") == 7) continue;

		Procedure p;

		p.his_index = getInt(procXml, "index");
		p.code = getString(procXml, "code");
		p.date = getString(procXml, "datePerformed");
		p.financingSource = static_cast<FinancingSource>(getInt(procXml, "financingSource"));
		p.notes = getString(procXml, "note");

		auto diagnosisXml = procXml->FirstChildElement("nhis:diagnosis");

		if (diagnosisXml) {

			p.diagnosis.icd = diagnosisXml->FirstChildElement("nhis:code")->FirstAttribute()->ValueStr();

			auto note = procXml->FirstChildElement("nhis:diagnosis")->FirstChildElement("nhis:note");

			if (note) {
				p.diagnosis.additional_descr = note->FirstAttribute()->ValueStr();
			}
		}

		HISProcedureResult affectedTeeth = HISHistoryAlgorithms::getHisToothContainer(*procXml);

		if (p.code.type() == ProcedureType::FullExam) {
			list.teeth = affectedTeeth.getToothContainer();
		}
		else {
			p.HIS_fetched_result = affectedTeeth;
		}

		list.procedures.addProcedure(p);
	}

	list.LPK = getString(
		docHandle.
		FirstChild().				//message
		Child(1).					//contents
		Child(1).					//results
		FirstChildElement("nhis:performer").ToElement(),
		"pmi"
	);

	auto patientXml = docHandle.
		FirstChild().				//message
		Child(1).					//contents
		Child(1).					//results
		FirstChildElement("nhis:subject").ToElement();	//subject

	patient.type = static_cast<Patient::Type>(getInt(patientXml, "identifierType"));
	patient.id = (getString(patientXml, "identifier"));

	//no foriegner parsing
	if (patient.type > Patient::LNCH) {
		m_callback = nullptr;
		return;
	}

	patient.HIRBNo = getString(patientXml, "nhifInsuranceNumber");
	patient.birth = getString(patientXml, "birthDate");
	patient.sex = static_cast<Patient::Sex>(getInt(patientXml, "gender") - 1);
	patient.FirstName = getFormattedStrName(patientXml->FirstChildElement("nhis:name"), "given");
	patient.MiddleName = getFormattedStrName(patientXml->FirstChildElement("nhis:name"), "middle");
	patient.LastName = getFormattedStrName(patientXml->FirstChildElement("nhis:name"), "family");
	patient.city = getInt(patientXml->FirstChildElement("nhis:address"), "ekatte");
	patient.address = getString(patientXml->FirstChildElement("nhis:address"), "line");
	patient.phone = getString(patientXml, "phone");

	m_callback(list, patient);

	m_callback = nullptr;

}
