#include "HISHistoryAlgorithms.h"
#include <TinyXML/tinyxml.h>
#include <algorithm>
#include <functional>
#include <set>
#include "Model/Dental/ToothUtils.h"

std::vector<Procedure> HISHistoryAlgorithms::getProcedures(TiXmlDocument& doc)
{
	TiXmlHandle docHandle(&doc);

	auto contents = docHandle.
		FirstChild(). //message
		Child(1);	  //contents
	//parsing procedures
	std::vector<Procedure> procedures;

	std::vector<ToothIndex> teethIndexes;

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
			if (elementName == "nhis:tooth" && 
				(
					p.code.type() != ProcedureType::FullExam && 
					p.code.type() != ProcedureType::Depuratio
				)
			)
			{
				auto index = ToothUtils::getToothFromNhifNum(pXml.Child(y).Child(0).ToElement()->Attribute("value"));

				index.supernumeral = pXml.Child(y).Child(1).ToElement()->ValueStr() == "nhis:supernumeralIndex";

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
				auto diagElement = pXml.Child(y).ToElement();
	
				auto& d = p.diagnosis;
				
				d.icd = diagElement->FirstChildElement("nhis:code")->FirstAttribute()->ValueStr();

				auto noteElement = diagElement->FirstChildElement("nhis:note");

				if (noteElement) {
					d.additional_descr = noteElement->FirstAttribute()->ValueStr();
				}
			}

			y++;
		}

		if (std::string(pXml.Child(3).ToElement()->Attribute("value")) == "7") {

			p.notes = "ГРЕШНО ВЪВЕДЕНА";
		}

		//separating procedures according to teeth affected
		for (int j = 0; j < teethIndexes.size(); j++)
		{
			if (j != 0) { procedures.push_back(procedures.back()); };

			auto& idx = teethIndexes[j];

			procedures.back().affectedTeeth = idx;

		}

		teethIndexes.clear();
	}
	
	std::sort(procedures.begin(), procedures.end(), [](const Procedure& a, const Procedure& b) { return a.date > b.date; });

	return procedures;
}

struct Ranges {

	std::vector<int> splints;
	std::vector<int> bridges;
	std::vector<int> pontics;

	void removeIdx(int index) {
		
		auto exists = [=](int other) {
			return index == other;
		};

		std::vector<int>::iterator it;

		it = std::remove_if(splints.begin(), splints.end(), exists);
		it = std::remove_if(bridges.begin(), bridges.end(), exists);
		it = std::remove_if(pontics.begin(), pontics.end(), exists);

	}

	void sortRanges() 
	{
		std::sort(splints.begin(), splints.end());
		std::sort(bridges.begin(), bridges.end());
		std::sort(pontics.begin(), pontics.end());
	}

};

//string to lambda map placed into it's own fn because intellisense has trouble parsing it
void deserializeStatusCode(Tooth& tooth, Ranges&, const std::string& code);

ToothContainer HISHistoryAlgorithms::getToothStatus(const TiXmlElement& node)
{
	return getHisToothContainer(node).getToothContainer();
}

HISToothContainer HISHistoryAlgorithms::getHisToothContainer(const TiXmlElement& node)
{
	HISToothContainer result;

	//iterrating teeth
	for (
		auto toothXml = node.FirstChildElement("nhis:tooth");
		toothXml != nullptr;
		toothXml = toothXml->NextSiblingElement("nhis:tooth")
		)
	{

		HISTooth tooth;

		//getting tooth index
		tooth.idx = ToothUtils::getToothFromHisNum(
			toothXml->FirstChildElement()->FirstAttribute()->ValueStr(),
			toothXml->FirstChildElement("nhis:supernumeralIndex") // exists only if tooth is dsn
		);

		//getting conditions
		for (
			auto conditionXml = toothXml->FirstChildElement("nhis:condition");
			conditionXml != nullptr;
			conditionXml = conditionXml->NextSiblingElement("nhis:condition")
			)
		{
			tooth.conditions.push_back(conditionXml->FirstChildElement()->FirstAttribute()->ValueStr());

		}

		result.push_back(tooth);
	}

	return result;
}


std::vector<HisSnapshot> HISHistoryAlgorithms::getDentalHistory(TiXmlDocument& doc)
{
	TiXmlHandle docHandle(&doc);

	std::vector<std::pair<Date, ToothContainer>> snapshots;

	struct DataToParse
	{
		Date date;
		std::string procedureName;
		std::string diagnosis {"Няма"};
		std::string note;
		FinancingSource financing{ 4 };
		std::vector<int> affectedTeeth;
		std::unordered_map<ToothIndex, std::vector<std::string>> statuses;
	};

	std::vector<DataToParse> tempData;

	//PARSING DATA

	//iterating dental procedures
	for (
		auto dentalProcedure = docHandle.FirstChild("nhis:message").FirstChild("nhis:contents").FirstChild("nhis:dentalProcedure").ToElement();
		dentalProcedure != nullptr;
		dentalProcedure = dentalProcedure->NextSiblingElement("nhis:dentalProcedure")	
	)
	{


		//continue, if it is entered in error
		if (dentalProcedure->FirstChildElement("nhis:status")->FirstAttribute()->IntValue() == 7) continue;

		//continue if there are no teeth in the procedure
		if (!dentalProcedure->FirstChildElement("nhis:tooth")) continue;

		DataToParse data;

		//getting the date
		Date date = dentalProcedure->FirstChildElement("nhis:datePerformed")->FirstAttribute()->ValueStr();

		data.statuses.reserve(32);
		data.affectedTeeth.reserve(32);

		data.date = date;
		data.procedureName = dentalProcedure->FirstChildElement("nhis:code")->FirstAttribute()->ValueStr();
		data.procedureName = ProcedureCode(data.procedureName).name();
		data.financing = static_cast<FinancingSource>(dentalProcedure->FirstChildElement("nhis:financingSource")->FirstAttribute()->IntValue());

		auto diagElement = dentalProcedure->FirstChildElement("nhis:diagnosis");

		if (diagElement) {	

			Diagnosis d;

			d.icd = diagElement->FirstChildElement("nhis:code")->FirstAttribute()->ValueStr();
			
			auto noteElement = diagElement->FirstChildElement("nhis:note");

			if (noteElement) {
				d.additional_descr = noteElement->FirstAttribute()->ValueStr();
			}

			data.diagnosis = d.getDiagnosisText();
		}

		auto noteElement = dentalProcedure->FirstChildElement("nhis:note");

		if (noteElement) {
			data.note = noteElement->FirstAttribute()->ValueStr();
		}
		
		//iterating teeth
		for (
			auto toothXml = dentalProcedure->FirstChildElement("nhis:tooth");
			toothXml != nullptr;
			toothXml = toothXml->NextSiblingElement("nhis:tooth")
		)
		{
			//getting the tooth index
			ToothIndex toothIndex = ToothUtils::getToothFromHisNum(
				toothXml->FirstChildElement("nhis:toothIndex")->FirstAttribute()->ValueStr(),
				toothXml->FirstChildElement("nhis:supernumeralIndex")
			);

			data.affectedTeeth.push_back(toothIndex.index);

			//getting conditions
			std::vector<std::string> conditions;

			for(
				auto condition = toothXml->FirstChildElement("nhis:condition"); 
				condition!=nullptr; 
				condition = condition->NextSiblingElement("nhis:condition")
			)
			{
				conditions.push_back(condition->FirstChildElement("nhis:code")->FirstAttribute()->ValueStr());
			}

			data.statuses[toothIndex] = conditions;
			
			
		}


		//determening point of insertion and inserting
		auto it = std::upper_bound(tempData.begin(), tempData.end(), data, 
			[](const DataToParse& lhs, const DataToParse& rhs){ return lhs.date < rhs.date; });

		tempData.insert(it, data);
		
	}
	
	//copying previous statuses
	if (tempData.size() > 1) {

		for (int i = 1; i < tempData.size(); i++)
		{
			for (auto& pair : tempData[i - 1].statuses)
			{

				if (tempData[i].statuses.count(pair.first)) continue;

				tempData[i].statuses[pair.first] = pair.second;
			}
		}
	}

	//DESERIALIZING:
	std::vector<HisSnapshot> result;

	result.reserve(tempData.size());

	for (auto& data : tempData)
	{
		result.emplace_back();

		auto& snapshot = result.back();

		snapshot.date = data.date;
		snapshot.affected_teeth = data.affectedTeeth;
		snapshot.procedure_name = data.procedureName;
		snapshot.procedure_diagnosis = data.diagnosis;
		snapshot.procedure_note = data.note;
		snapshot.financing = data.financing;

		auto& teeth = snapshot.teeth;
		
		Ranges ranges;

		for (auto &[toothIdx, conditions] : data.statuses)
		{
			teeth[toothIdx.index].setStatus(Dental::HasSupernumeral, toothIdx.supernumeral);

			auto& tooth = teeth.at(toothIdx);

			tooth.setStatus(Dental::Temporary, toothIdx.temp);

			for (auto& code : conditions)
			{
				deserializeStatusCode(tooth, ranges, code);
			}
		}

		ranges.sortRanges();

		teeth.setStatus(ranges.splints, Dental::StatusType::General, Dental::Splint, true);
		teeth.setStatus(ranges.bridges, Dental::StatusType::General, Dental::Bridge, true);

		//in case extraction is not set on pontics:
		for (auto idx : ranges.pontics) if (teeth[idx].canHaveACrown()) teeth[idx].setStatus(Dental::Missing, true);

	}

	return result;
}

using namespace Dental;

void deserializeStatusCode(Tooth& tooth, Ranges& r, const std::string& code)
{
    static std::map<std::string, std::function<void(Tooth& tooth, Ranges& r)>> lambdaMap
	{
        { "E",	[](Tooth& tooth, Ranges&) mutable	{ tooth.setStatus(Dental::Missing, true); } },
        { "T",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Calculus, true); } },
        { "K",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Crown, true); } },
        { "B",	[](Tooth& tooth, Ranges& r) mutable { r.bridges.push_back(tooth.index()); r.pontics.push_back(tooth.index()); } },
        { "Kb",	[](Tooth& tooth, Ranges& r) mutable { r.bridges.push_back(tooth.index()); } },
        { "O",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Restoration, true); } },
        { "C",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Caries, true); } },
        { "Oo",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Restoration,Surface::Occlusal); } },
        { "Om",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Restoration, Surface::Medial);  } },
        { "Od",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Restoration, Surface::Distal);  } },
        { "Ob",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Restoration, Surface::Buccal); } },
        { "Ol",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Restoration, Surface::Lingual);  } },
        { "Oc",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Restoration, Surface::Cervical);  } },
        { "Co",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Caries, Surface::Occlusal); } },
        { "Cm",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Caries, Surface::Medial);  } },
        { "Cd",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Caries, Surface::Distal);  } },
        { "Cb",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Caries, Surface::Buccal); } },
        { "Cl",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Caries, Surface::Lingual);  } },
        { "Cc",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Caries, Surface::Cervical);  } },
		{ "M1",	[](Tooth& tooth, Ranges&) mutable { tooth.setMobility(Dental::I, true); } },
        { "M2",	[](Tooth& tooth, Ranges&) mutable { tooth.setMobility(Dental::II, true); } },
        { "M3",	[](Tooth& tooth, Ranges&) mutable { tooth.setMobility(Dental::III, true); } },
        { "X",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Denture, true); } },
        { "R",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Root, true); } },
        { "Rc",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::RootCanal, true); } },
        { "Rp",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Post, true); } },
        { "H",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Healthy, true); } },
        { "I",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Implant, true); } },
        { "Re",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Impacted, true); } },
        { "G",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::ApicalLesion, true); } },
        { "P",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Pulpitis, true); } },
        { "F",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Fracture, true); } },
        { "Pa",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Periodontitis, true); } },
        { "D",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::HasSupernumeral, true); } },
        { "S",	[](Tooth& tooth, Ranges& r) mutable { r.splints.push_back(tooth.index()); } }
	};

	if(lambdaMap.count(code)) lambdaMap[code](tooth, r);
}


/*
ToothContainer HISHistoryAlgorithms::getToothStatus(TiXmlDocument& doc)
{

	TiXmlHandle docHandle(&doc);

	auto contents = docHandle.
		FirstChild(). //message
		Child(1);	  //contents


	//getting most recent status

	ToothContainer teeth;

	Ranges ranges;

	auto status = contents.Child(0);

	//0-31 normal; 32-63 supernumeral
	bool statusSet[64] = {false};

	//parsing the status
	for (int i = 0; status.Child(i).ToElement() != nullptr; i++) //tooth
	{

		auto [index, isTemp, isDsn] = ToothUtils::getToothFromNhifNum(status.Child(i).Child(0).ToElement()->Attribute("value")); //toothIndex

		if (status.Child(i).Child(1).ToElement()->ValueStr() == "nhis:supernumeralIndex")  //supernumeralIndex
		{
			isDsn = true;
			teeth[index].setStatus(Dental::HasSupernumeral, true);
		}

		auto &tooth = isDsn ? teeth[index].dsn.tooth() : teeth[index];

		//filtering out the temporary-permanent duplication
		{
			int statusSetIdx = isDsn ? index + 32 : index;

			if (statusSet[statusSetIdx]) {
				tooth.removeStatus();
				ranges.removeIdx(index);
			}
			else {
				statusSet[statusSetIdx] = true;
			}
		}

		int conditionArrayIdx = isDsn ? 3 : 2;

		tooth.temporary.set(isTemp);

		while (status.Child(i).Child(conditionArrayIdx).ToElement())
		{
			auto code = status						//dentalStatus
				.Child(i)							//tooth
				.Child(conditionArrayIdx)			//condition
				.FirstChildElement()				//code
				.ToElement()->Attribute("value");	//value

			deserializeStatusCode(tooth, ranges, code);

			conditionArrayIdx++;
		}

	}

	teeth.setStatus(ranges.splints, Dental::GeneralStatus, Dental::Splint, true);
	teeth.setStatus(ranges.bridges, Dental::GeneralStatus, Dental::Bridge, true);

	//in case extraction is not set on pontics:
	for (auto idx : ranges.pontics) if (teeth[idx].canHaveACrown()) teeth[idx].setStatus(Dental::Missing, false);

	return teeth;

}
*/
