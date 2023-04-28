#include "EDental.h"

#include <array>
#include <map>

#include "Model/User.h"
#include <TinyXML/tinyxml.h>
#include <qdebug.h>
#include "View/ModalDialogBuilder.h"
#include "Model/Dental/ToothContainer.h"

bool EDental::Open::sendRequest(
    const AmbList& ambSheet, 
    const Patient& patient, 
    std::function<void(const std::string&, const std::vector<int>&)> nrnCallback
)
{
	m_callback = nrnCallback;

	std::string contents;

	bool isNhif = ambSheet.isNhifSheet();

	bool adverseConditions = isNhif && ambSheet.nhifData.isUnfavourable;

	contents.reserve(4000);

	contents +=
		"<nhis:dentalTreatment>"
			+ bind("lrn", ambSheet.lrn)
			//+ bind("basedOn", ambSheet.basedOn) //needs closing functionality
			+ bind("treatmentStart", ambSheet.date)
			//+ bind("treatmentEnd", ambSheet.time.to8601(ambSheet.getDate())) //TO IMPLEMENT!!!
			+ bind("adverseConditions", adverseConditions)
			+ bind("rhifAreaNumber", patient.city.getRhif() + patient.city.getHealthRegion())
			+ "<nhis:medicalStatus />" //TO IMPLEMENT!!!
			+ getProcedures(ambSheet.procedures, ambSheet.teeth)
		+"</nhis:dentalTreatment>"
		+ HisService::subject(patient)
		+ HisService::performer()
	;		

	return HisService::sendRequestToHis(contents);
}

std::string EDental::Open::getProcedures(const ProcedureContainer& procedures, const ToothContainer& teeth)
{

	std::string result;

	result.reserve(1000);

	int sequence = 0;

	ToothContainer teethChanged = teeth;
	Date lastProcedureDate(Date::currentDate());

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

			p.applyProcedure(teethChanged);

			for (int i = begin; i <= end; i++)
			{
				result += getToothStatus(teethChanged.at(i));
			}
		}

		if (p.code.oldCode() == 101)
		{
			for (auto& tooth : teeth)
			{
				result += getToothStatus(tooth);
			}

		}

		result += bind("note", p.notes);

		if (p.diagnosis.index() != 0) {

			result += "<nhis:diagnosis>";
			result += bind("code", p.diagnosis.index());
			result += bind("note", p.diagnosis.additionalDescription, true);
			result += "</nhis:diagnosis>";

		}

		result += "</nhis:dentalProcedure>";

		lastProcedureDate = p.date;

	}

	//to implement this, his_idx needs to be stored somewhere
	//result += HisService::getResultingStatusAsProcedure(teethChanged, lastProcedureDate);

	return result;

}

void EDental::Open::parseReply(const std::string& reply)
{
	auto errors = getErrors(reply);

	if (errors.size()) {
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

	std::vector<int> procedureIndex;

	auto contents = docHandle.
		FirstChild(). //message
		Child(1);	  //contents


	//dentalProcedures
	for (int i = 3; contents.Child(i).ToElement(); i++)
	{
		//index
		procedureIndex.push_back(std::stoi(contents.Child(i).Child(1).ToElement()->Attribute("value")));
	}

	m_callback(nrnStr, procedureIndex);

}


bool EDental::Augment::sendRequest(const AmbList& ambSheet, const Patient& patient, std::function<void(const std::map<int, int>& procedureIndexes)> callback)
{
	m_callback = callback;

	std::string contents;

	bool isNhif = ambSheet.isNhifSheet();

	bool adverseConditions = isNhif && ambSheet.nhifData.isUnfavourable;

	contents.reserve(4000);

	std::string resultingDocuments = "<nhis:documents>";

	for(auto& ref : ambSheet.referrals) resultingDocuments += bind("nrnReferral", ref.nrn);

	//implement prescriptions and medical notices here

	resultingDocuments += "</nhis:documents>";

	contents +=
		"<nhis:dentalTreatment>"
		+ bind("nrnDental", ambSheet.nrn)
		//+ bind("basedOn", ambSheet.basedOn)
		+ bind("treatmentStart", ambSheet.date)
		+ bind("adverseConditions", adverseConditions)
		+ bind("rhifAreaNumber", patient.city.getRhif() + patient.city.getHealthRegion())
		+ "<nhis:medicalStatus />" //TO IMPLEMENT!!!
		+ resultingDocuments
		+ getProcedures(ambSheet.procedures, ambSheet.teeth)
		+ "</nhis:dentalTreatment>"
		+ HisService::subject(patient)
		+ HisService::performer()
		;

	return HisService::sendRequestToHis(contents);
}

std::string EDental::Augment::getProcedures(const ProcedureContainer& procedures, const ToothContainer& teeth)
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

		if (p.his_index) {
			result += bind("index", p.his_index);
		}

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

			p.applyProcedure(teethChanged);

			for (int i = begin; i <= end; i++)
			{
				result += getToothStatus(teethChanged.at(i));
			}
		}

		if (p.code.oldCode() == 101)
		{
			for (auto& tooth : teeth)
			{
				result += getToothStatus(tooth);
			}

		}

		result += bind("note", p.notes, true);

		if (p.diagnosis.index() != 0) {

			result += "<nhis:diagnosis>";
			result += bind("code", p.diagnosis.index());
			result += bind("note", p.diagnosis.additionalDescription, true);
			result += "</nhis:diagnosis>";

		}

		result += "</nhis:dentalProcedure>";

	}

	sequence = 999;

	for (auto& p : procedures.removedProcedures()) {

		sequence++;

		result += "<nhis:dentalProcedure>";

		result += bind("sequence", sequence);

		result += bind("index", p.his_index);

		result += bind("code", p.code.code());

		result += bind("status", 7);

		result += bind("type", static_cast<int>(p.code.type()));

		result += bind("datePerformed", p.date.to8601());

		result += bind("financingSource", static_cast<int>(p.financingSource));

		result += "</nhis:dentalProcedure>";

	}
	


	return result;
	
}

void EDental::Augment::parseReply(const std::string& reply)
{
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

	std::map<int, int> procedureIndex;

	//dentalProcedures
	for (int i = 2; contents.Child(i).ToElement(); i++)
	{
		//sequence
		auto sequence = std::stoi(contents.Child(i).Child(0).ToElement()->Attribute("value"));
		//index
		auto hisIdx = std::stoi(contents.Child(i).Child(1).ToElement()->Attribute("value"));

		if (sequence >= 999) continue;

		procedureIndex[sequence-1] = hisIdx;
	}

	m_callback(procedureIndex);
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
		m_callback(true); return;
	}

	m_callback(false);

}


bool EDental::GetStatus::sendRequest(const Patient& patient, std::function<void(const ToothContainer&, const ProcedureContainer&)> callback)
{
	m_callback = callback;

	std::string contents =
		bind("identifierType", patient.type) +
		bind("identifier", patient.id) +
		bind("practiceNumber", User::practice().rziCode)
		;

	return HisService::sendRequestToHis(contents);
}


void EDental::GetStatus::parseReply(const std::string& reply)
{
	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		m_callback = nullptr;
		return;
	}


	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto status = docHandle.
		FirstChild(). //message
		Child(1).	  //contents
		Child(0);     //dentalStatus

	//getting most recent status

	ToothContainer teeth;

	std::vector<int> splints;

	static std::map<std::string, std::function<void(int idx)>> lambdaMap
	{
		{"E",	[&teeth](int idx) mutable { teeth[idx].extraction.set(true); }},
		{"T",	[&teeth](int idx) mutable { teeth[idx].calculus.set(true); }},
		{"K",	[&teeth](int idx) mutable { teeth[idx].crown.set(true); }},
		{"B",	[&teeth](int idx) mutable { teeth[idx].bridge.set(true); teeth[idx].bridge.position = BridgePos::Middle; teeth[idx].extraction.set(true); }},
		{"O",	[&teeth](int idx) mutable { teeth[idx].obturation.set(true); }},
		{"C",	[&teeth](int idx) mutable { teeth[idx].caries.set(true); }},
		{"Oo",	[&teeth](int idx) mutable { teeth[idx].obturation.set(true, Surface::Occlusal); }},
		{"Om",	[&teeth](int idx) mutable { teeth[idx].obturation.set(true, Surface::Medial);  }},
		{"Od",	[&teeth](int idx) mutable { teeth[idx].obturation.set(true, Surface::Distal);  }},
		{"Ob",	[&teeth](int idx) mutable { teeth[idx].obturation.set(true, Surface::Buccal); }},
		{"Ol",	[&teeth](int idx) mutable { teeth[idx].obturation.set(true, Surface::Lingual);  }},
		{"Oc",	[&teeth](int idx) mutable { teeth[idx].obturation.set(true, Surface::Cervical);  }},
		{"Co",	[&teeth](int idx) mutable { teeth[idx].caries.set(true, Surface::Occlusal); }},
		{"Cm",	[&teeth](int idx) mutable { teeth[idx].caries.set(true, Surface::Medial);  }},
		{"Cd",	[&teeth](int idx) mutable { teeth[idx].caries.set(true, Surface::Distal);  }},
		{"Cb",	[&teeth](int idx) mutable { teeth[idx].caries.set(true, Surface::Buccal); }},
		{"Cl",	[&teeth](int idx) mutable { teeth[idx].caries.set(true, Surface::Lingual);  }},
		{"Cc",	[&teeth](int idx) mutable { teeth[idx].caries.set(true, Surface::Cervical);  }},
		{"M1",	[&teeth](int idx) mutable { teeth[idx].mobility.set(true); teeth[idx].mobility.degree = Degree::First; }},
		{"M2",	[&teeth](int idx) mutable { teeth[idx].mobility.set(true); teeth[idx].mobility.degree = Degree::Second; }},
		{"M3",	[&teeth](int idx) mutable { teeth[idx].mobility.set(true); teeth[idx].mobility.degree = Degree::Third; }},
		{"X",	[&teeth](int idx) mutable { teeth[idx].denture.set(true); }},
		{"R",	[&teeth](int idx) mutable { teeth[idx].root.set(true); }},
		{"Rc",	[&teeth](int idx) mutable { teeth[idx].endo.set(true); }},
		{"Rp",	[&teeth](int idx) mutable { teeth[idx].post.set(true); }},
		{"H",	[&teeth](int idx) mutable { teeth[idx].healthy.set(true); }},
		{"I",	[&teeth](int idx) mutable { teeth[idx].implant.set(true); }},
		{"Re",	[&teeth](int idx) mutable { teeth[idx].impacted.set(true); }},
		{"G",	[&teeth](int idx) mutable { teeth[idx].lesion.set(true); }},
		{"P",	[&teeth](int idx) mutable { teeth[idx].pulpitis.set(true); }},
		{"F",	[&teeth](int idx) mutable { teeth[idx].fracture.set(true); }},
		{"Pa",	[&teeth](int idx) mutable { teeth[idx].periodontitis.set(true); }},
		{"S",	[&splints](int idx) mutable { splints.push_back(idx); }}
	};



	for (int i = 0; status.Child(i).ToElement() != nullptr; i++) //tooth
	{

		auto [index, temp, dsn] = ToothUtils::getToothFromNhifNum(status.Child(i).Child(0).ToElement()->Attribute("value")); //toothIndex

		teeth[index].temporary.set(temp);
		
		for (int y = 2; status.Child(i).Child(y).ToElement() != nullptr; y++) 
		{
			auto condition = status.Child(i).Child(y).ToElement();

			if (condition->ValueStr() != "nhis:condition") {
				dsn = true;
				continue;
			}

			auto code = condition->FirstChildElement()->Attribute("value");

			lambdaMap[code](index);

		}

	}

	teeth.setStatus(splints, StatusCode::FiberSplint, true);

	m_callback(teeth, {});

	m_callback = nullptr;

}
