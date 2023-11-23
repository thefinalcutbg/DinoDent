#include "GetFileService.h"
#include "View/ModalDialogBuilder.h"
#include "TinyXML/tinyxml.h"
#include "Network/Base64Convert.h"
#include <map>
#include <utility>

void GetFileService::processPISReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback(reply);
		m_callback = nullptr;
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto fileData =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement()			  //table
		.Child(1)						  //first row
		.FirstChildElement().ToElement(); //notifData;

	if (fileData)
	{
		m_callback(Base64Convert::decode(getTextNullCheck(fileData)));
	}
	else
	{
		m_callback("");
	}
}

bool GetFileService::sendRequest(
    const std::string& fileHash, 
    const std::string& rziCode, 
    std::function<void(const std::string& file)> callback
)
{
	m_callback = callback;

		auto query =
	"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
            "xmlns:ns3=\"http://pis.technologica.com/ws/\">"
            "<ns3:user><ns3:msp>" + rziCode + "</ns3:msp></ns3:user>"
        "<ns3:from_clause>REPORT_FILE_BYTES</ns3:from_clause>"
    "<ns3:where_clause>"
        "<ns1:and>"
            "<ns1:filter>"
                "<ns1:xfy oper=\"=\">"
                    "<ns1:fcolumn>file_id</ns1:fcolumn>"       
                    "<ns1:fvalue>" + fileHash + "</ns1:fvalue>"
                "</ns1:xfy>"
            "</ns1:filter>"
        "</ns1:and>"
    "</ns3:where_clause>"
    "</ns3:query>"
	;

    return PisService::sendRequest(query, SOAPAction::View);

}

bool GetAmbHashes::sendRequest(const std::string& rziCode, int year, decltype(m_callback) callback)
{
	m_callback = callback;

	auto query =
	"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
	"xmlns:ns3=\"http://pis.technologica.com/ws/\">"

		"<ns3:user><ns3:msp>" + rziCode + "</ns3:msp></ns3:user>"

		"<ns3:select_clause>"
				"<ns1:scolumn>FILE_PROCESS_REPORT</ns1:scolumn>" //errors
				"<ns1:scolumn>REPORT_DATE_FROM</ns1:scolumn>"	 //date
				"<ns1:scolumn>FILE_ID</ns1:scolumn>"			 //id hash
			"</ns3:select_clause>"
		
		"<ns3:from_clause>REPORT_FILE2</ns3:from_clause>"

			"<ns3:where_clause>"
				"<ns1:and>"
					"<ns1:filter>"
						"<ns1:xfy oper=\"=\">"
							"<ns1:fcolumn>TYPE_CODE</ns1:fcolumn>"
							"<ns1:fvalue>AMB_DENT</ns1:fvalue>"
						"</ns1:xfy>"
					"</ns1:filter>"

					"<ns1:filter>"
						"<ns1:xfy oper=\"=\">"
							"<ns1:fcolumn>PRACT_UIN</ns1:fcolumn>"
							"<ns1:fvalue>"+User::doctor().LPK+"</ns1:fvalue>"
						"</ns1:xfy>"
					"</ns1:filter>"
			 "<ns1:or>"
		;

	//12 ORs because of the dumb nhif api
	for (int i = 1; i < 13; i++)
	{
		query += 
			"<ns1:filter>"
				"<ns1:xfy oper=\"=\">"
					"<ns1:fcolumn>REPORT_DATE_FROM</ns1:fcolumn>"
					"<ns1:fvalue>";

		query += std::to_string(year);
		query += "-";
		query += FreeFn::leadZeroes(i, 2);
		query +=
					"-01 00:00:00</ns1:fvalue>"
				"</ns1:xfy>"
			"</ns1:filter>"
			;
	}
		
	query+=
					"</ns1:or>"
				"</ns1:and>"
			"</ns3:where_clause>"
		"<ns3:orderby_clause>"
			"<ns1:ocolumn sort=\"ASC\">FILE_DATE</ns1:ocolumn>"
		"</ns3:orderby_clause>"
	"</ns3:query>"
	;

	return PisService::sendRequest(query, SOAPAction::View);
}

void GetAmbHashes::processPISReply(const std::string& reply)
{
	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto table =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement();			  //table

	std::map<std::pair<int, int>, std::string> months_to_hashes;

	//i is 1, since 0 is the table header (td)
	for (int i = 1; ; i++)
	{
		auto row = table.Child(i); //table row

		if (row.ToElement() == nullptr) {
			break;
		}

		if (getTextNullCheck(row.Child(0).ToElement()).size()) continue;


		Date date = getTextNullCheck(row.Child(1).ToElement());

		auto key = std::make_pair(date.year, date.month);

		months_to_hashes[key] = getTextNullCheck(row.Child(2).ToElement());
	}

	std::stack<std::string> result;
	// iterating lists in reverse order because of the HIRBNo-s
	for (auto& pair : months_to_hashes) {

		result.push(pair.second);
	}

	m_callback(result);

	m_callback = nullptr;
}