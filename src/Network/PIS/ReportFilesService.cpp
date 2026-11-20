#include "ReportFilesService.h"
#include <TinyXML/tinyxml.h>
#include "View/ModalDialogBuilder.h"
#include "Model/User.h"

void ReportFilesService::processPISReply(const std::string& reply)
{
	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto table =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement();			  //table

	std::vector<ReportFile> result;

	auto periodFormat = [](const std::string& dateStr)
	{
		if (dateStr.size() < 12) return std::string();
		
		Date date(dateStr);

		return date.getMonthName() + " " + std::to_string(date.year) + " г.";
	};

	//i is 1, since 0 is the table header (td)
	for (int i = 1; ; i++)
	{
		auto row = table.Child(i); //table row

		if (row.ToElement() == nullptr) {
			break;
		}


		result.emplace_back(
			ReportFile{
				getTextNullCheck(row.Child(0).ToElement()),							//upload date
				getTextNullCheck(row.Child(1).ToElement()),							//file description
				periodFormat(getTextNullCheck(row.Child(2).ToElement())),			//doc date	    
				User::getNameFromLPK(getTextNullCheck(row.Child(3).ToElement())),	//lpk
				getTextNullCheck(row.Child(4).ToElement()),							//status
				getTextNullCheck(row.Child(5).ToElement())							//errors
			}
		);

	}

	m_callback(result);
}


void ReportFilesService::sendRequest(const std::string& rziCode, std::function<void(const std::vector<ReportFile>&)> callback)
{
	m_callback = callback;

	auto query =
	"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
	"xmlns:ns3=\"http://pis.technologica.com/ws/\">"

		"<ns3:user><ns3:msp>" + rziCode + "</ns3:msp></ns3:user>"

		"<ns3:select_clause>"
				"<ns1:scolumn>FILE_DATE</ns1:scolumn>"
				"<ns1:scolumn>TYPE_DESCR_BG</ns1:scolumn>"
				"<ns1:scolumn>REPORT_DATE_FROM</ns1:scolumn>"
				"<ns1:scolumn>PRACT_UIN</ns1:scolumn>"
				"<ns1:scolumn>STATUS_DESCR_BG</ns1:scolumn>"
				"<ns1:scolumn>FILE_PROCESS_REPORT</ns1:scolumn>"
			"</ns3:select_clause>"
		
		"<ns3:from_clause>REPORT_FILE2</ns3:from_clause>"

		"<ns3:orderby_clause>"
			"<ns1:ocolumn sort=\"DESC\">FILE_DATE</ns1:ocolumn>"
		"</ns3:orderby_clause>"
	"</ns3:query>"
	;

	PisService::sendRequest(query, SOAPAction::View);
}
