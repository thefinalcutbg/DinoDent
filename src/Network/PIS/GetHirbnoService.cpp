#include "GetHirbnoService.h"
#include <TinyXML/tinyxml.h>
#include "Model/FreeFunctions.h"
#include "View/ModalDialogBuilder.h"

void GetHirbnoService::sendRequest(int personType, const std::string& id, std::function<void(const std::string&)> callback)
{
	m_callback = callback;

	std::string tag = personTypeArr[personType];

	auto query =
		"<ns3:query xmlns:ns1=\"http://pis.technologica.com/views/\" "
		"xmlns:ns3=\"http://pis.technologica.com/ws/\">"
		"<ns3:user>"
		"<ns3:" + tag + ">" + id + "</ns3:" + tag + ">"
		"</ns3:user>"
		"<ns3:from_clause>ACTIVE_HB</ns3:from_clause>"
		"</ns3:query>"
		;

	PisService::sendRequest(query, SOAPAction::View);
}

void GetHirbnoService::processPISReply(const std::string& reply)
{
	if (reply.empty()) {
		m_callback = nullptr;
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	TiXmlElement* hirbno =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement()			  //table
		.Child(1)						  //table row
		.FirstChildElement().ToElement(); //table data



	if (!hirbno) {
		ModalDialogBuilder::showMessage("Не е намерена активна здравна книжка");
		return;
	}

	std::string result = hirbno->GetText();

	//PIS has hirb numbers leading zeroes all messed up, so we have to normalize them
	result.size() > 8 ?
		result = result.substr(result.size() - 8)
		:
		result = FreeFn::leadZeroes(result, 8);

	m_callback(result);
}
