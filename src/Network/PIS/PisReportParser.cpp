#include "PisReportParser.h"
#include <functional>
#include <unordered_map>
#include <TinyXML/tinyxml.h>
#include "Model/FreeFunctions.h"
#include "Model/Dental/ToothUtils.h"

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

		patient.city = Ekatte(
			std::stoi(listXml->Attribute("RHIFCode")),
			std::stoi(listXml->Attribute("healthRegionCode"))
		);

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
		
			auto& tooth = list.teeth.at(idx);

			if (idx.supernumeral) list.teeth[idx.index].setStatus(Dental::HasSupernumeral);
			if (idx.temp) list.teeth[idx.index].setStatus(Dental::Temporary);

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

					if (ToothUtils::getToothType(p.tooth_idx.index) == Dental::Frontal) {
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
		{ "E",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::Missing); }},
		{ "T",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::Temporary); } },
		{ "K",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::Crown); } },
		{ "O",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::Restoration); } },
		{ "C",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::Caries); } },
		{ "I",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::Mobility); tooth.m_degree = Dental::I; } },
		{ "II",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::Mobility); tooth.m_degree = Dental::II; } },
		{ "III",[](Tooth& tooth) mutable { tooth.setStatus(Dental::Mobility); tooth.m_degree = Dental::III; } },
		{ "X",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::Denture); } },
		{ "R",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::Root); } },
		{ "Impl.",[](Tooth& tooth) mutable { tooth.setStatus(Dental::Implant); } },
		{ "G",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::ApicalLesion); } },
		{ "P",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::Pulpitis); } },
		{ "F",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::Fracture); } },
		{ "Pa",	[](Tooth& tooth) mutable { tooth.setStatus(Dental::Periodontitis); } },
	};

	if (lambdaMap.count(code)) lambdaMap[code](tooth);
}
