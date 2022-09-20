#include "DentalActivitiesService.h"
#include "Model/Date.h"

#include "TinyXML/tinyxml.h"

#include "Model/Dental/ToothUtils.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Dental/NhifProcedures.h"


bool DentalActivitiesService::sendRequest(int personType, const std::string& patientId, std::function<void(const std::optional<std::vector<Procedure>>&)> callback)
{
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

	if (reply.empty()) {
		m_callback({});
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	if (!doc.RootElement()) { return; }

	TiXmlHandle docHandle(&doc);

	auto table =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement();			  //table

	std::vector<Procedure> result;

	//i is 1, since 0 is the table header (td)
	for (int i = 1; ; i++)
	{
		auto row = table.Child(i); //table row

		if (!row.ToElement()) {
			break;
		}

		auto [tooth, temp] = ToothUtils::getArrayIdxAndTemp(std::stoi(row.Child(5).ToElement()->GetText()));

		result.emplace_back(
			NhifProcedures::getTemplateByCode(std::stoi(row.Child(2).ToElement()->GetText())),
			Date::getDateFromXmlFormat(row.Child(0).ToElement()->GetText()),
			row.Child(4).ToElement()->GetText(), //diagnosis
			tooth,
			temp
		);

	}

	m_callback(result);

}
