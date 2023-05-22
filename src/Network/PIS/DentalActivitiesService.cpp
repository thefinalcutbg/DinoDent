#include "DentalActivitiesService.h"
#include "Model/Date.h"

#include "TinyXML/tinyxml.h"

#include "Model/Dental/ToothUtils.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Dental/NhifProcedures.h"

bool DentalActivitiesService::sendRequest(
		int personType, 
		const std::string& patientId, 
		bool showDialogs,
		decltype(m_callback) callback
	)
{
	this->show_dialogs = showDialogs;

	m_callback = callback;

	std::string tag = personTypeArr[personType];

	auto query =
		"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
		"xmlns:ns3=\"http://pis.technologica.com/ws/\">"
			"<ns3:user>"
				"<ns3:" + tag + ">" + patientId + "</ns3:" + tag + ">"
			"</ns3:user>"

			"<ns3:from_clause>INYEAR_DENTAL_ACTS</ns3:from_clause>"

			"<ns3:orderby_clause>"
				"<ns1:ocolumn sort=\"DESC\">ACTIVITY_DATE</ns1:ocolumn>"
			"</ns3:orderby_clause>"

		"</ns3:query>"
		;

	return PisService::sendRequest(query, SOAPAction::View);

}

void DentalActivitiesService::parseReply(const std::string& reply)
{
	if (reply.empty()) { m_callback({});  return; }

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	if (!doc.RootElement()) { return; }

	TiXmlHandle docHandle(&doc);

	auto table =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement();			  //table

	std::vector<Procedure> procedures;
	//i is 1, since 0 is the table header (td)
	for (int i = 1; ; i++)
	{
		auto row = table.Child(i); //table row

		if (!row.ToElement()) {
			break;
		}

		auto toothProp = 

		procedures.emplace_back(Procedure(std::stoi(row.Child(2).ToElement()->GetText())));

		procedures.back().date = Date(row.Child(0).ToElement()->GetText());
		procedures.back().diagnosis.description = row.Child(4).ToElement()->GetText(); //diagnosis
		procedures.back().tooth_idx = ToothUtils::getToothFromNhifNum(row.Child(5).ToElement()->GetText());
		procedures.back().notes = row.Child(1).ToElement()->GetText();
		procedures.back().financingSource = FinancingSource::NHIF;
		procedures.back().db_source = Procedure::DatabaseSource::PIS;
	}

	m_callback(procedures);

}
