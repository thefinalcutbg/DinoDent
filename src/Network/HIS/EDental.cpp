#include "EDental.h"

#include "Model/User.h"
#include <TinyXML/tinyxml.h>
#include "View/ModalDialogBuilder.h"
#include "Model/Dental/ToothContainer.h"
#include "HISHistoryAlgorithms.h"
#include "Model/FreeFunctions.h"

bool EDental::Open::sendRequest(
    const AmbList& ambSheet,
    const Patient& patient,
	bool patientIsSigner,
    decltype(m_callback) nrnCallback,
    std::function<void(const std::vector<unsigned char>& sig_bitmap, const std::string& sig_data)> sig_callback
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
			+ bind("basedOn", ambSheet.basedOn) //needs closing functionality
			+ bind("treatmentStart", FreeFn::LocalToUTC(ambSheet.date))
			+ bind("treatmentEnd", FreeFn::LocalToUTC(ambSheet.treatment_end))
			+ bind("adverseConditions", adverseConditions)
			+ bind("rhifAreaNumber", patient.city.getRhif() + patient.city.getHealthRegion())
			+ HisService::getMedicalStatus(patient)
			+ getProcedures(ambSheet.procedures, ambSheet.teeth, ambSheet.date)
		+"</nhis:dentalTreatment>"
    ;

    //signed from previous attempt
    auto sig_data = ambSheet.signature_data;
    auto sig_bitmap = ambSheet.signature_bitmap;

    if (User::signatureTablet().getHisIdx())
    {
        //signing for a first time
        if(sig_data.empty() || !patientIsSigner){

            auto sign_pair = HisService::generatePatientSignature(contents, patient, patientIsSigner);

            sig_data = sign_pair.first;
            sig_bitmap = sign_pair.second;
        }

        //no patient signature
        if (sig_data.empty()){

            if(!ModalDialogBuilder::askDialog("Амбулаторният лист не е подписан. Желаете ли да го изпратите към НЗИС без подпис на пациента?"))
            {
                m_callback = nullptr;
                return false;
            }
        }
        //signing successful
        else {sig_callback(sig_bitmap, sig_data);}

    }


	contents += HisService::subject(patient)
		+ HisService::performer(isNhif);

    return HisService::sendRequestToHis(contents, sig_data);
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
			m_callback = nullptr;
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

bool EDental::Augment::sendRequest(
    const AmbList& ambSheet,
    const Patient& patient,
	bool patientIsSigner,
    bool removeAutoStatus,
    decltype(m_callback) callback,
    std::function<void(const std::vector<unsigned char>& sig_bitmap, const std::string& sig_data)> sig_callback
    )
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
		+ bind("basedOn", ambSheet.basedOn) //needs closing functionality
		+ bind("treatmentStart", FreeFn::LocalToUTC(ambSheet.date))
		+ bind("treatmentEnd", FreeFn::LocalToUTC(ambSheet.treatment_end))
		+ bind("adverseConditions", adverseConditions)
		+ bind("rhifAreaNumber", patient.city.getRhif() + patient.city.getHealthRegion())
		+ HisService::getMedicalStatus(patient)
		+ resultingDocuments
		+ getProcedures(ambSheet.procedures, ambSheet.teeth, ambSheet.date, removeAutoStatus)
		+ "</nhis:dentalTreatment>";

    //signed from previous attempt
    auto sig_data = ambSheet.signature_data;
    auto sig_bitmap = ambSheet.signature_bitmap;

    if (User::signatureTablet().getHisIdx())
    {
        //signing for a first time
        if(sig_data.empty() || !patientIsSigner){

            auto sign_pair = HisService::generatePatientSignature(contents, patient, patientIsSigner);

            sig_data = sign_pair.first;
            sig_bitmap = sign_pair.second;
        }

        //no patient signature
        if (sig_data.empty()){

            if(!ModalDialogBuilder::askDialog("Амбулаторният лист не е подписан. Желаете ли да го изпратите към НЗИС без подпис на пациента?"))
            {
                m_callback = nullptr;
                return false;
            }
        }
        //signing successful
        else {sig_callback(sig_bitmap, sig_data);}

    }

    return HisService::sendRequestToHis(contents, sig_data);
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

void EDental::GetStatusAndProcedures::parseReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback({}, {});
		m_callback = nullptr;
		return;
	}

	auto errors = getErrors(reply);

	if (errors.size()) {
		if (show_dialogs) {
			ModalDialogBuilder::showError(errors);
		}
		m_callback({}, {});
		m_callback = nullptr;
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	auto procedures = getProceduresFromHis(doc.FirstChild()->FirstChildElement("nhis:contents"));

	std::reverse(procedures.begin(), procedures.end());

    m_callback(procedures, HISHistoryAlgorithms::getDentalHistory(doc));

	m_callback = nullptr;
}

bool EDental::GetStatusAndProcedures::sendRequest(const Patient& patient,bool showDialogs, decltype(m_callback) callback)
{
	this->show_dialogs = showDialogs;

	m_callback = callback;

	std::string contents =
		bind("identifierType", patient.getHisIdentifierType()) +
		bind("identifier", patient.id) +
		bind("fromDate", "2008-01-01") +
		bind("toDate", Date::currentDate().to8601()) +
		bind("practiceNumber", User::practice().rziCode)
		;

	return HisService::sendRequestToHis(contents);
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
	list.treatment_end = FreeFn::UTCToLocal(getString(ambXml, "treatmentEnd"));
	list.basedOn = getString(ambXml, "basedOn");
	list.nhifData.isUnfavourable = getBool(ambXml, "adverseConditions");
	list.his_updated = true;

	for (auto& p : getProceduresFromHis(ambXml)) {
		list.procedures.addProcedure(p);
	}

	//getting the teeth status from full examps
	for (
		auto procXml = ambXml->FirstChildElement("nhis:dentalProcedure");
		procXml != nullptr;
		procXml = procXml->NextSiblingElement("nhis:dentalProcedure")
		)
	{
		//entered in error
		if (getInt(procXml, "status") == 7) continue;

		ProcedureCode code = getString(procXml, "code");

		HISProcedureResult affectedTeeth = HISHistoryAlgorithms::getHisToothContainer(*procXml);

		if (code.type() == ProcedureType::FullExam) {
			list.teeth = affectedTeeth.getToothContainer();
		}
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

	if (patient.type == 5) { patient.type = Patient::EU; }

	//non supported id types
	if (patient.type > 5 || patient.type == 4) {
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
