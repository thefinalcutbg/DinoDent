#include "EDental.h"

#include <array>
#include <map>

#include "Model/User.h"
#include <TinyXML/tinyxml.h>
#include "View/ModalDialogBuilder.h"
#include "Model/Dental/ToothContainer.h"

bool EDental::Open::sendRequest(
    const AmbList& ambSheet, 
    const Patient& patient, 
	decltype(m_callback) nrnCallback
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
			+ getProcedures(ambSheet.procedures, ambSheet.teeth, ambSheet.date)
		+"</nhis:dentalTreatment>"
		+ HisService::subject(patient)
		+ HisService::performer()
	;		

	return HisService::sendRequestToHis(contents);
}

std::string EDental::Open::getProcedures(const ProcedureContainer& procedures, const ToothContainer& teeth, const Date& treatmentStartDate)
{

	std::string result;

	result.reserve(1000);

	ToothContainer teethChanged = teeth;

	result += HisService::initialStatusAsProcedure(teeth, treatmentStartDate);
	int sequence = 1;

	for (auto& p : procedures)
	{
		sequence++;
		result += HisService::getProcedure(p, teeth, teethChanged, sequence);
	}

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
		auto index = std::stoi(contents.Child(i).Child(1).ToElement()->Attribute("value"));
		auto sequence = std::stoi(contents.Child(i).Child(0).ToElement()->Attribute("value"));

		if (sequence == 1) continue; //the first procedure is always the initial status
		
		//index
		procedureIndex.push_back(index);
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
		+ getProcedures(ambSheet.procedures, ambSheet.teeth, ambSheet.date)
		+ "</nhis:dentalTreatment>"
		+ HisService::subject(patient)
		+ HisService::performer()
		;

	return HisService::sendRequestToHis(contents);
}

std::string EDental::Augment::getProcedures(const ProcedureContainer& procedures, const ToothContainer& teeth, const Date& treatmentStartDate)
{
	
	std::string result;

	result.reserve(1000);

	result += HisService::initialStatusAsProcedure(teeth, treatmentStartDate, true);

	int sequence = 1;

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

		result += bind("type", p.code.hisType());

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

		if (hisIdx == 1) continue;

		if (sequence >= 999) continue;

		procedureIndex[sequence-2] = hisIdx;
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


bool EDental::GetStatus::sendRequest(const Patient& patient, std::function<void(const ToothContainer&)> callback)
{
	m_callback = callback;

	std::string contents =
		bind("identifierType", patient.type) +
		bind("identifier", patient.id) +
		bind("fromDate", Date::currentDate().to8601()) +
		bind("toDate", Date::currentDate().to8601()) +
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

	//ModalDialogBuilder::showMultilineDialog(reply);

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto contents = docHandle.
		FirstChild(). //message
		Child(1);	  //contents


	//getting most recent status

	ToothContainer teeth;

	std::vector<int> splints;

	static std::map<std::string, std::function<void(int idx, Tooth& tooth)>> lambdaMap
	{
		{"E",	[&teeth](int idx, Tooth& tooth) mutable { tooth.extraction.set(true); }},
		{"T",	[&teeth](int idx, Tooth& tooth) mutable { tooth.calculus.set(true); }},
		{"K",	[&teeth](int idx, Tooth& tooth) mutable { tooth.crown.set(true); }},
		{"B",	[&teeth](int idx, Tooth& tooth) mutable { tooth.bridge.set(true); tooth.bridge.position = BridgePos::Middle; teeth[idx].extraction.set(true); }},
		{"O",	[&teeth](int idx, Tooth& tooth) mutable { tooth.obturation.set(true); }},
		{"C",	[&teeth](int idx, Tooth& tooth) mutable { tooth.caries.set(true); }},
		{"Oo",	[&teeth](int idx, Tooth& tooth) mutable { tooth.obturation.set(true, Surface::Occlusal); }},
		{"Om",	[&teeth](int idx, Tooth& tooth) mutable { tooth.obturation.set(true, Surface::Medial);  }},
		{"Od",	[&teeth](int idx, Tooth& tooth) mutable { tooth.obturation.set(true, Surface::Distal);  }},
		{"Ob",	[&teeth](int idx, Tooth& tooth) mutable { tooth.obturation.set(true, Surface::Buccal); }},
		{"Ol",	[&teeth](int idx, Tooth& tooth) mutable { tooth.obturation.set(true, Surface::Lingual);  }},
		{"Oc",	[&teeth](int idx, Tooth& tooth) mutable { tooth.obturation.set(true, Surface::Cervical);  }},
		{"Co",	[&teeth](int idx, Tooth& tooth) mutable { tooth.caries.set(true, Surface::Occlusal); }},
		{"Cm",	[&teeth](int idx, Tooth& tooth) mutable { tooth.caries.set(true, Surface::Medial);  }},
		{"Cd",	[&teeth](int idx, Tooth& tooth) mutable { tooth.caries.set(true, Surface::Distal);  }},
		{"Cb",	[&teeth](int idx, Tooth& tooth) mutable { tooth.caries.set(true, Surface::Buccal); }},
		{"Cl",	[&teeth](int idx, Tooth& tooth) mutable { tooth.caries.set(true, Surface::Lingual);  }},
		{"Cc",	[&teeth](int idx, Tooth& tooth) mutable { tooth.caries.set(true, Surface::Cervical);  }},
		{"M1",	[&teeth](int idx, Tooth& tooth) mutable { tooth.mobility.set(true); teeth[idx].mobility.degree = Degree::First; }},
		{"M2",	[&teeth](int idx, Tooth& tooth) mutable { tooth.mobility.set(true); teeth[idx].mobility.degree = Degree::Second; }},
		{"M3",	[&teeth](int idx, Tooth& tooth) mutable { tooth.mobility.set(true); teeth[idx].mobility.degree = Degree::Third; }},
		{"X",	[&teeth](int idx, Tooth& tooth) mutable { tooth.denture.set(true); }},
		{"R",	[&teeth](int idx, Tooth& tooth) mutable { tooth.root.set(true); }},
		{"Rc",	[&teeth](int idx, Tooth& tooth) mutable { tooth.endo.set(true); }},
		{"Rp",	[&teeth](int idx, Tooth& tooth) mutable { tooth.post.set(true); }},
		{"H",	[&teeth](int idx, Tooth& tooth) mutable { tooth.healthy.set(true); }},
		{"I",	[&teeth](int idx, Tooth& tooth) mutable { tooth.implant.set(true); }},
		{"Re",	[&teeth](int idx, Tooth& tooth) mutable { tooth.impacted.set(true); }},
		{"G",	[&teeth](int idx, Tooth& tooth) mutable { tooth.lesion.set(true); }},
		{"P",	[&teeth](int idx, Tooth& tooth) mutable { tooth.pulpitis.set(true); }},
		{"F",	[&teeth](int idx, Tooth& tooth) mutable { tooth.fracture.set(true); }},
		{"Pa",	[&teeth](int idx, Tooth& tooth) mutable { tooth.periodontitis.set(true); }},
		{"D",	[&teeth](int idx, Tooth& tooth) mutable { tooth.dsn.set(true); }},
		{"S",	[&splints](int idx, Tooth& tooth) mutable { splints.push_back(idx); }}
	};


	auto status = contents.Child(0);

	//parsing the status
	for (int i = 0; status.Child(i).ToElement() != nullptr; i++) //tooth
	{

		auto [index, temp, dsn] = ToothUtils::getToothFromNhifNum(status.Child(i).Child(0).ToElement()->Attribute("value")); //toothIndex

		teeth[index].temporary.set(temp);
		
		for (int y = 2; status.Child(i).Child(y).ToElement() != nullptr; y++) 
		{
			auto condition = status.Child(i).Child(y).ToElement();

			if (condition->ValueStr() == "nhis:supernumeralIndex")  //supernumeralIndex
			{
				teeth[index].setStatus(StatusCode::Dsn, true);
				teeth[index].dsn.tooth().setStatus(StatusCode::Healthy, true); //only temporary, until NHIS starts returning the supernumeral tooth status 
				dsn = true;
				continue;
			}
			
			auto code = condition->FirstChildElement()->Attribute("value"); //condition

			auto& tooth = dsn ? teeth[index].dsn.tooth() : teeth[index];

			lambdaMap[code](index, tooth);

		}

	}

	teeth.setStatus(splints, StatusType::general, StatusCode::FiberSplint, true, false);

	m_callback(teeth);

	m_callback = nullptr;

}

void EDental::GetProcedures::parseReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback({}, show_dialogs);
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

	TiXmlHandle docHandle(&doc);

	auto contents = docHandle.
		FirstChild(). //message
		Child(1);	  //contents
	//parsing procedures
	std::vector<Procedure> procedures;

	std::vector<ToothUtils::ToothProcedureCode> teethIndexes;

	for (int i = 1; contents.Child(i).ToElement() != nullptr; i++)
	{
		auto pXml = contents.Child(i);


		procedures.emplace_back();

		auto& p = procedures.back();

		p.date = Date(pXml.Child(5).ToElement()->Attribute("value"));
		p.code = ProcedureCode(pXml.Child(2).ToElement()->Attribute("value"));
		p.financingSource = static_cast<FinancingSource>(std::stoi(pXml.Child(6).ToElement()->Attribute("value")));
		p.db_source = Procedure::DatabaseSource::HIS;

		int y = 7;

		while (true)
		{

			if (pXml.Child(y).ToElement() == nullptr) break;

			auto elementName = pXml.Child(y).ToElement()->ValueStr();

			//parsing teeth indexes:
			if (elementName == "nhis:tooth" && p.code.type() != ProcedureType::general)
			{
				auto index = ToothUtils::getToothFromNhifNum(pXml.Child(y).Child(0).ToElement()->Attribute("value"));

				index.hyperdontic = pXml.Child(y).Child(1).ToElement()->ValueStr() == "nhis:supernumeralIndex";

				teethIndexes.push_back(index);

			}

			//parsing notes
			if (elementName == "nhis:note")
			{
				p.notes = pXml.Child(y).ToElement()->Attribute("value");
			}
			
	

			//parsing diagnosis
			if (elementName == "nhis:diagnosis")
			{
				auto diagHandle = pXml.Child(y);
				p.diagnosis = std::stoi(diagHandle.Child(0).ToElement()->Attribute("value"));

				if (diagHandle.Child(1).ToElement() != nullptr)
				{
					p.diagnosis.additionalDescription = diagHandle.Child(1).ToElement()->Attribute("value");
				}


			}

			y++;
		}

		if (std::string(pXml.Child(3).ToElement()->Attribute("value")) == "7") {

			p.notes = "АНУЛИРАНА";
		}

		//separating procedures according to teeth affected
		for (int j = 0; j < teethIndexes.size(); j++)
		{
			if (j != 0) { procedures.push_back(procedures.back()); };

			auto& idx = teethIndexes[j];

			procedures.back().hyperdontic = idx.hyperdontic;
			procedures.back().temp = idx.temporary;
			procedures.back().tooth = idx.tooth;
		}

		teethIndexes.clear();
	}

	m_callback(procedures, this->show_dialogs);

	m_callback = nullptr;

}

bool EDental::GetProcedures::sendRequest(const Patient& patient, bool showDialogs, decltype(m_callback) callback)
{

	this->show_dialogs = showDialogs;

	m_callback = callback;

	std::string contents =
		bind("identifierType", patient.type) +
		bind("identifier", patient.id) +
		bind("fromDate", "2023-05-01") +
		bind("toDate", Date::currentDate().to8601()) +
		bind("practiceNumber", User::practice().rziCode)
		;

	return HisService::sendRequestToHis(contents);
}
