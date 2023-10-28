#include "PisReportParser.h"
#include <functional>
#include <unordered_map>
#include <TinyXML/tinyxml.h>
#include "Model/FreeFunctions.h"

void deserializeNhifStatus(Tooth& tooth, const std::string& code);

PisReportsForImport PisReportParser::parse(const std::string& xmlReport)
{
	TiXmlDocument doc;
	doc.Parse(xmlReport.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	PisReportsForImport result;

	for (
		auto listXml = docHandle
		.FirstChild("report")
		.FirstChild("dentalCareServices")
		.FirstChild("dentalCareService").ToElement();

		listXml != nullptr;

		listXml = listXml->NextSiblingElement("dentalCareService")
		)
	{

		Patient patient;
		patient.type = static_cast<Patient::Type>(std::stoi(listXml->Attribute("personType")));

		if (patient.type == Patient::Type::EU) continue;

		patient.id = listXml->Attribute("personIdentifier");
		patient.FirstName = listXml->Attribute("personFirstName");
		patient.LastName = listXml->Attribute("personLastName");

		patient.HIRBNo = listXml->Attribute("HIRBNo");

		if (listXml->Attribute("personMiddleName")) {
			patient.MiddleName = listXml->Attribute("personMiddleName");
		}

		if (patient.type == Patient::EGN) {
			patient.sex = patient.getSexFromEgn(patient.id);
			patient.birth = Date::getBirthdateFromEgn(patient.id);
		}
		else {
			patient.birth = Date{listXml->Attribute("birthDate")};
		}

		AmbList list;

		list.lrn = FreeFn::getUuid();

		list.LPK = doc.FirstChildElement("report")->Attribute("dentistPersonalCode");

		list.nhifData.setSpecFromNhif(listXml->Attribute("specificationType"));

		list.nhifData.isUnfavourable = listXml->Attribute("unfavourableCondition") == "1";

		std::string listNumber = listXml->Attribute("ambulatorySheetNo");

		if (listNumber.size() == 12) {
			list.nrn = listNumber;
			list.his_updated = true;
		}
		else
		{
			list.number = std::stoi(listNumber);
		}

		//list.nhifData.specification = TO IMPLEMENT!

		TiXmlHandle teethHandle(listXml);

		//ITERATION OF TEETH
		for (
			auto toothXml = teethHandle.FirstChild("teeth").FirstChild("tooth").ToElement();
			toothXml != nullptr;
			toothXml = toothXml->NextSiblingElement("tooth")
			) 
		{

			ToothIndex idx = ToothUtils::getToothFromNhifNum(toothXml->FirstAttribute()->ValueStr());
		
			auto& tooth = idx.supernumeral ? 
				list.teeth[idx.index].dsn.tooth() 
				: 
				list.teeth[idx.index];

			if (idx.supernumeral) list.teeth[idx.index].setStatus(StatusCode::Dsn);

			//DESERIALIZING STATUSES
			for (
				auto statusXml = toothXml->FirstChildElement("toothStatuses")->FirstChildElement("toothStatus");
				statusXml != nullptr;
				statusXml = statusXml->NextSiblingElement("toothStatus")
				) 
			{
				
				deserializeNhifStatus(tooth, statusXml->FirstAttribute()->ValueStr());
			}
		}

		//DESERIALIZING SERVICES

		TiXmlHandle serviceHandle(listXml);

		for (
			auto serviceXml = serviceHandle.FirstChild("services").FirstChild("service").ToElement();
			serviceXml != nullptr;
			serviceXml = serviceXml->NextSiblingElement("service")
			)
		{
			Procedure p;

			p.date = Date{ serviceXml->Attribute("date") };
			p.financingSource = FinancingSource::NHIF;

			p.code = std::stoi(serviceXml->Attribute("activityCode"));
			p.diagnosis.description = serviceXml->Attribute("diagnosis");

			if (p.code.isToothSpecific())
			{
				p.tooth_idx = ToothUtils::getToothFromNhifNum(serviceXml->Attribute("toothCode"));

				if (!p.tooth_idx.isValid()) continue;

				if (p.code.type() == ProcedureType::obturation) {

					RestorationData restoration;

					if (ToothUtils::getToothType(p.tooth_idx.index) == ToothType::Frontal) {
						restoration.surfaces[3] = true;
					}
					else {
						restoration.surfaces[0] = true;
					};

					p.result = restoration;
				}
			}

			if (p.code.type() == ProcedureType::denture)
			{
				ConstructionRange range;
				
				if (p.code.oldCode() == 832) {
					p.result = ConstructionRange{
						.tooth_begin = 1,
						.tooth_end = 14
					};
				}
				else
				{
					p.result = ConstructionRange{
						.tooth_begin = 17,
						.tooth_end = 30
					};
				}
			}

			list.procedures.addProcedure(p);
		}

		//cannot deduce the amblist date
		if (list.procedures.empty()) continue;

		list.date = list.procedures[0].date.to8601() + "T00:00:00";

		//assuming the list has only nhif procedures
		if (list.nrn.size()) {
			for (int i = 0; i < list.procedures.size(); i++)
				list.procedures[i].his_index = i + 1;
		}

		result.push_back(std::make_pair(patient, list));
	}

	return result;
}

void deserializeNhifStatus(Tooth& tooth, const std::string& code)
{
	static std::map<std::string, std::function<void(Tooth& tooth)>> lambdaMap
	{
		{ "E",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Extraction); }},
		{ "T",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Temporary); } },
		{ "K",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Crown); } },
		{ "O",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Obturation); } },
		{ "C",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Caries); } },
		{ "I",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Mobility); tooth.mobility.degree = Degree::First; } },
		{ "II",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Mobility); tooth.mobility.degree = Degree::Second; } },
		{ "III",[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Mobility); tooth.mobility.degree = Degree::Third; } },
		{ "X",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Denture); } },
		{ "R",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Root); } },
		{ "Impl.",[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Implant); } },
		{ "G",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::ApicalLesion); } },
		{ "P",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Pulpitis); } },
		{ "F",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Fracture); } },
		{ "Pa",	[](Tooth& tooth) mutable { tooth.setStatus(StatusCode::Periodontitis); } },
	};

	if (lambdaMap.count(code)) lambdaMap[code](tooth);
}