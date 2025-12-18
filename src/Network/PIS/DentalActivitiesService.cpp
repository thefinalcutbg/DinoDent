#include "DentalActivitiesService.h"
#include "Model/Date.h"

#include "TinyXML/tinyxml.h"

#include "Model/Dental/ToothUtils.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/Patient.h"

bool DentalActivitiesService::sendRequest(
		const Patient& p,
		bool showDialogs,
		decltype(m_callback) callback
	)
{
	this->show_dialogs = showDialogs;

	m_callback = callback;

	int personType = p.type;

	if (p.foreigner && p.foreigner->isEHIC()) {
		personType++;
	}

	std::string tag = getPersonIdTag(p);

	auto query =
		"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
		"xmlns:ns3=\"http://pis.technologica.com/ws/\">"
			"<ns3:user>"
				"<ns3:" + tag + ">" + p.id + "</ns3:" + tag + ">"
			"</ns3:user>"

			"<ns3:from_clause>INYEAR_DENTAL_ACTS</ns3:from_clause>"

			"<ns3:orderby_clause>"
				"<ns1:ocolumn sort=\"DESC\">ACTIVITY_DATE</ns1:ocolumn>"
			"</ns3:orderby_clause>"

		"</ns3:query>"
		;

	return PisService::sendRequest(query, SOAPAction::View);

}

void DentalActivitiesService::processPISReply(const std::string& reply)
{
	if (reply.empty()) { 
		m_callback({});  
		m_callback = nullptr;
		return; 
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	if (!doc.RootElement()) { 
		m_callback({});
		m_callback = nullptr;
		return; 
	}

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

        procedures.emplace_back();
        procedures.back().code = std::stoi(getTextNullCheck(row.Child(2).ToElement()));
		procedures.back().date = Date(row.Child(0).ToElement()->GetText());
        auto diagnosis = getTextNullCheck(row.Child(4).ToElement());
		
		procedures.back().diagnosis.icd = diagnosis;

		if (!procedures.back().diagnosis.icd.isValid()) {
			procedures.back().diagnosis.additional_descr = diagnosis;
		}

        procedures.back().affectedTeeth = ToothUtils::getToothFromNhifNum(getTextNullCheck(row.Child(5).ToElement()));
        procedures.back().notes = getTextNullCheck(row.Child(1).ToElement());
		procedures.back().financingSource = FinancingSource::NHIF;
		procedures.back().db_source = Procedure::DatabaseSource::PIS;
	}

	m_callback(procedures);

}
