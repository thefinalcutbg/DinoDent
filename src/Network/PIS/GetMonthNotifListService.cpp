#include "GetMonthNotifListService.h"
#include <TinyXML/tinyxml.h>


FinancialDocType getNotifType(const std::string& type) {

	const char* typeStr[3]{
		"C_FDOC_INV",
		"C_FDOC_DTNOTE",
		"C_FDOC_CTNOTE"
	};

	for (int i = 0; i < 3; i++) {

		if (type == typeStr[i])
			return static_cast<FinancialDocType>(i);
	}

	return FinancialDocType::Unknown;

}


void GetMonthNotifListService::sendRequest(const std::string& rziCode, std::function<void(const std::optional<std::vector<MonthNotifRow>>&)> callback)
{
	m_callback = callback;

	auto query = 
	"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
				   "xmlns:ns3=\"http://pis.technologica.com/ws/\">"
			"<ns3:user><ns3:msp>" + rziCode + "</ns3:msp></ns3:user>"
			"<ns3:select_clause>"
					"<ns1:scolumn>MONTHNOTIF_ID_HASH</ns1:scolumn>" //needed to get the actual data
					"<ns1:scolumn>ACC_DATE</ns1:scolumn>"			//the notif date
					"<ns1:scolumn>FILE_TYPE_CODE</ns1:scolumn>"		//notification type
					"<ns1:scolumn>MSP_CODE</ns1:scolumn>"			//RZI code
					"<ns1:scolumn>UIN_NUMBER</ns1:scolumn>"			//LPK code
					"<ns1:scolumn>CLAIM_ID_HASH</ns1:scolumn>"		//has it been claimed?
			"</ns3:select_clause>"
			"<ns3:from_clause>MONTH_NOTIF_INFO</ns3:from_clause>"

			"<ns3:orderby_clause>"
                "<ns1:ocolumn sort=\"DESC\">ACC_DATE</ns1:ocolumn>"
           "</ns3:orderby_clause>"

		"</ns3:query>"
		;

	PisService::sendRequest(query, SOAPAction::View);
}


void GetMonthNotifListService::parseReply(const std::string& reply)
{

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto table =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement();			  //table

	std::vector<MonthNotifRow> result;

	//i is 1, since 0 is the table header (td)
	for (int i = 1; ; i++)
	{
		auto row = table.Child(i); //table row

		if (row.ToElement() == nullptr) {
			break;
		}


		result.emplace_back(
			MonthNotifRow{
				row.Child(0).ToElement()->GetText(),								//id hash
				Date::getDateFromXmlFormat(row.Child(1).ToElement()->GetText()),	//date
				getNotifType(getTextNullCheck(row.Child(2).ToElement())),						//type
				getTextNullCheck(row.Child(3).ToElement()),								//rzi
				getTextNullCheck(row.Child(4).ToElement()),								//uin
				(row.Child(5).ToElement()->GetText() != nullptr)					//claimed
			}
		);

	}

	m_callback(result);
}
