#include "HISHistoryAlgorithms.h"
#include <TinyXML/tinyxml.h>
#include <algorithm>
#include <functional>

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
			if (elementName == "nhis:tooth" && p.code.type() != ProcedureType::full_exam)
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
				auto diagHandle = pXml.Child(y);
				p.diagnosis = std::stoi(diagHandle.Child(0).ToElement()->Attribute("value"));

				if (diagHandle.Child(1).ToElement() != nullptr)
				{
					p.diagnosis.description = diagHandle.Child(1).ToElement()->Attribute("value");
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

			procedures.back().tooth_idx = idx;

		}

		teethIndexes.clear();
	}
	
	return procedures;
}

struct Ranges {
	std::vector<int> splints;
	std::vector<int> bridges;
	std::vector<int> pontics;
};

//string to lambda map placed into it's own fn because intellisense has trouble parsing it
void deserializeStatusCode(Tooth& tooth, Ranges& r, const std::string& code);

ToothContainer HISHistoryAlgorithms::getToothStatus(TiXmlDocument& doc)
{
	TiXmlHandle docHandle(&doc);

	auto contents = docHandle.
		FirstChild(). //message
		Child(1);	  //contents

	struct ToothCondition {
		ToothIndex idx;
		std::vector<std::string> conditions;
	};

	struct Snapshot {
		Date date;
		std::vector<ToothCondition> teeth;
	};

	std::vector<Snapshot> history;

	for (int i = 1; contents.Child(i).ToElement() != nullptr; i++)
	{
		TiXmlHandle p = contents.Child(i);

		//checks if entered in error:
		if (std::string(p.Child(3).ToElement()->Attribute("value")) == "7") continue;

		//getting date
		Date date = std::string(p.Child(5).ToElement()->Attribute("value"));

		//getting code and assigning date day to 0 (the exam shows always the initial status)
		auto code = std::string(p.Child(2).ToElement()->Attribute("value"));

		//evil exam date hack:
		if (code == "D-01-001" || code == "D-01-003") {
			date.day = 0;//because the exam always shows the initial date
		}

		Snapshot snapshot;
		snapshot.date = date;

		//iterrating teeth
		for (int y = 0; p.Child(y).ToElement() != nullptr; y++)
		{
			if (p.Child(y).ToElement()->ValueStr() != "nhis:tooth") continue;

			//getting tooth index
			auto index = ToothUtils::getToothFromNhifNum(p.Child(y).Child(0).ToElement()->Attribute("value"));

			index.supernumeral = p.Child(y).Child(1).ToElement()->ValueStr() == "nhis:supernumeralIndex";

			//iterrating conditions

			std::vector<std::string> conditions;

			int conditionIndex = index.supernumeral ? 2 : 1;

			while (p.Child(y).Child(conditionIndex).ToElement() != nullptr)
			{
				conditions.push_back(p.Child(y).Child(conditionIndex).FirstChild().ToElement()->Attribute("value"));
				conditionIndex++;
			}

			snapshot.teeth.push_back(ToothCondition{ index, conditions });
		}

		if (snapshot.teeth.size()) history.push_back(snapshot);
	}

	std::sort(history.begin(), history.end(), [](const Snapshot& a, const Snapshot& b) { return a.date < b.date; });

	std::unordered_map<ToothIndex, std::vector<std::string>> statuses;
	std::unordered_map<ToothIndex, std::vector<std::string>> supernumeralStatuses;

	for (auto& snapshot : history) {

		for (auto& tooth : snapshot.teeth) {
			tooth.idx.supernumeral ?
				supernumeralStatuses[tooth.idx] = tooth.conditions
				:
				statuses[tooth.idx] = tooth.conditions;
		}
	}

	ToothContainer teeth;

	Ranges ranges;

	for (auto const& [index, conditions] : statuses)
	{
		auto& tooth = teeth[index.index];

		if (index.temp) tooth.temporary.set(true);

		for (auto& code : conditions)
		{
			deserializeStatusCode(tooth, ranges, code);
		}
	}

	teeth.setStatus(ranges.splints, StatusType::general, StatusCode::FiberSplint, true);
	teeth.setStatus(ranges.bridges, StatusType::general, StatusCode::Bridge, true);

	//in case extraction is not set on pontics:
	for (auto idx : ranges.pontics) if (teeth[idx].canHaveACrown()) teeth[idx].setStatus(StatusCode::Extraction, true);
	
	//setting supernumeral only if the normal tooth has dsn set to true
	for (auto const& [index, conditions] : supernumeralStatuses)
	{
		if (!teeth[index.index].dsn) continue;

		auto& tooth = teeth[index.index].dsn.tooth();

		if (index.temp) tooth.temporary.set(true);

		for (auto& code : conditions)
		{
			deserializeStatusCode(tooth, ranges, code);
		}
	}

	return teeth;
}

/*
//if somewhere in the future NHIS fix their API, this implementation will be used
ToothContainer HISHistoryAlgorithms::getToothStatus(TiXmlDocument& doc)
{
	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto contents = docHandle.
		FirstChild(). //message
		Child(1);	  //contents


	//getting most recent status

	ToothContainer teeth;

	Ranges ranges;

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

			deserializeStatusCode(tooth, ranges, code);
		}

	}

	teeth.setStatus(ranges.splints, StatusType::general, StatusCode::FiberSplint, true);
	teeth.setStatus(ranges.bridges, StatusType::general, StatusCode::Bridge, true);

	//in case extraction is not set on pontics:
	for (auto idx : ranges.pontics) if (teeth[idx].canHaveACrown()) teeth[idx].extraction.set(false);


	m_callback(teeth);

	m_callback = nullptr;

}
*/

void deserializeStatusCode(Tooth& tooth, Ranges& r, const std::string& code)
{
	static std::map<std::string, std::function<void(Tooth& tooth, Ranges& r)>> lambdaMap
	{
		{ "E", [](Tooth& tooth, Ranges& r) mutable	{ tooth.extraction.set(true); }},
		{ "T",	[](Tooth& tooth, Ranges& r) mutable { tooth.calculus.set(true); } },
		{ "K",	[](Tooth& tooth, Ranges& r) mutable { tooth.crown.set(true); } },
		{ "B",	[](Tooth& tooth, Ranges& r) mutable { r.bridges.push_back(tooth.index); r.pontics.push_back(tooth.index); } },
		{ "Kb",	[](Tooth& tooth, Ranges& r) mutable { r.bridges.push_back(tooth.index); } },
		{ "O",	[](Tooth& tooth, Ranges& r) mutable { tooth.obturation.set(true); } },
		{ "C",	[](Tooth& tooth, Ranges& r) mutable { tooth.caries.set(true); } },
		{ "Oo",	[](Tooth& tooth, Ranges& r) mutable { tooth.obturation.set(true, Surface::Occlusal); } },
		{ "Om",	[](Tooth& tooth, Ranges& r) mutable { tooth.obturation.set(true, Surface::Medial);  } },
		{ "Od",	[](Tooth& tooth, Ranges& r) mutable { tooth.obturation.set(true, Surface::Distal);  } },
		{ "Ob",	[](Tooth& tooth, Ranges& r) mutable { tooth.obturation.set(true, Surface::Buccal); } },
		{ "Ol",	[](Tooth& tooth, Ranges& r) mutable { tooth.obturation.set(true, Surface::Lingual);  } },
		{ "Oc",	[](Tooth& tooth, Ranges& r) mutable { tooth.obturation.set(true, Surface::Cervical);  } },
		{ "Co",	[](Tooth& tooth, Ranges& r) mutable { tooth.caries.set(true, Surface::Occlusal); } },
		{ "Cm",	[](Tooth& tooth, Ranges& r) mutable { tooth.caries.set(true, Surface::Medial);  } },
		{ "Cd",	[](Tooth& tooth, Ranges& r) mutable { tooth.caries.set(true, Surface::Distal);  } },
		{ "Cb",	[](Tooth& tooth, Ranges& r) mutable { tooth.caries.set(true, Surface::Buccal); } },
		{ "Cl",	[](Tooth& tooth, Ranges& r) mutable { tooth.caries.set(true, Surface::Lingual);  } },
		{ "Cc",	[](Tooth& tooth, Ranges& r) mutable { tooth.caries.set(true, Surface::Cervical);  } },
		{ "M1",	[](Tooth& tooth, Ranges& r) mutable { tooth.mobility.set(true); tooth.mobility.degree = Degree::First; } },
		{ "M2",	[](Tooth& tooth, Ranges& r) mutable { tooth.mobility.set(true); tooth.mobility.degree = Degree::Second; } },
		{ "M3",	[](Tooth& tooth, Ranges& r) mutable { tooth.mobility.set(true); tooth.mobility.degree = Degree::Third; } },
		{ "X",	[](Tooth& tooth, Ranges& r) mutable { tooth.denture.set(true); } },
		{ "R",	[](Tooth& tooth, Ranges& r) mutable { tooth.root.set(true); } },
		{ "Rc",	[](Tooth& tooth, Ranges& r) mutable { tooth.endo.set(true); } },
		{ "Rp",	[](Tooth& tooth, Ranges& r) mutable { tooth.post.set(true); } },
		{ "H",	[](Tooth& tooth, Ranges& r) mutable { tooth.healthy.set(true); } },
		{ "I",	[](Tooth& tooth, Ranges& r) mutable { tooth.implant.set(true); } },
		{ "Re",	[](Tooth& tooth, Ranges& r) mutable { tooth.impacted.set(true); } },
		{ "G",	[](Tooth& tooth, Ranges& r) mutable { tooth.lesion.set(true); } },
		{ "P",	[](Tooth& tooth, Ranges& r) mutable { tooth.pulpitis.set(true); } },
		{ "F",	[](Tooth& tooth, Ranges& r) mutable { tooth.fracture.set(true); } },
		{ "Pa",	[](Tooth& tooth, Ranges& r) mutable { tooth.periodontitis.set(true); } },
		{ "D",	[](Tooth& tooth, Ranges& r) mutable { tooth.dsn.set(true); } },
		{ "S",	[](Tooth& tooth, Ranges& r) mutable { r.splints.push_back(tooth.index); } }
	};

	if(lambdaMap.count(code)) lambdaMap[code](tooth, r);
}