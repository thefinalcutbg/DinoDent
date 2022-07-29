#include "GetMonthNotifService.h"
#include <TinyXML/tinyxml.h>
#include "Network/Common/Base64Convert.h"


void GetMonthNotifService::sendRequest(const std::string& rziCode, const std::string& notifHash, std::function<void(const std::string&)> callback)
{
	m_callback = callback;

	auto query =
		"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
				   "xmlns:ns3=\"http://pis.technologica.com/ws/\">"

			"<ns3:user><ns3:msp>" + rziCode + "</ns3:msp></ns3:user>"

			"<ns3:select_clause>"
					"<ns1:scolumn>MONTHNOTIF_FILE_BYTES</ns1:scolumn>" //the actual data
			"</ns3:select_clause>"

			"<ns3:from_clause>MONTH_NOTIF_INFO</ns3:from_clause>"

			"<ns3:where_clause>"
					"<ns1:filter>"
						"<ns1:xfy oper=\"=\">"
							"<ns1:fcolumn>MONTHNOTIF_ID_HASH</ns1:fcolumn>"
							"<ns1:fvalue>" + notifHash + "</ns1:fvalue>"
						"</ns1:xfy>"
					"</ns1:filter>"
			"</ns3:where_clause>"

		"</ns3:query>"
		;

	PisService::sendRequest(query, SOAPAction::View);
}


void GetMonthNotifService::parseReply(const std::string& reply)
{
	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto notifData =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement()			  //table
		.Child(1)						  //first row
		.FirstChildElement().ToElement(); //notifData;

	if (notifData)
	{
		m_callback(Base64Convert::decode(getTextNullCheck(notifData)));
	}
	else
	{
		m_callback("");
	}
}
