#include "SendFileService.h"
#include <TinyXML/tinyxml.h>
#include "View/ModalDialogBuilder.h"
#include "Model/Financial/Invoice.h"
#include "Network/Base64Convert.h"
#include "Model/xml.h"


void SendFileService::sendInvoice(const Invoice& invoice, const std::string& rziCode)
{
	const char* purposeArr[4]{
		"FDOC_INV",
		"FDOC_DTNOTE",
		"FDOC_CTNOTE",
		""
	};

	auto invoiceStr = XML::getInvoice(invoice);

	auto query = 
		"<ns1:userFile xmlns:ns1=\"http://pis.technologica.com/ws/\" "
		"xmlns:ns2=\"http://pis.technologica.com/files/\">"

		"<ns1:user><ns1:msp>" + rziCode + "</ns1:msp></ns1:user>"

		"<ns1:file>"
		"<ns2:base64>" + Base64Convert::encode(invoiceStr.data(), invoiceStr.size()) + "</ns2:base64>"
		"<ns2:type>xml</ns2:type>"
		"<ns2:encoding>utf-8</ns2:encoding>"
		"</ns1:file>"
		"<ns1:purpose>" + purposeArr[static_cast<int>(invoice.type)] + "</ns1:purpose>"

		"</ns1:userFile>"
		;

	PisService::sendRequest(query, SOAPAction::Files);
}

void SendFileService::sendAmbReport(const std::string& data, const std::string& doctorEgn)
{
	auto query = 
		"<ns1:userFile xmlns:ns1=\"http://pis.technologica.com/ws/\" "
		"xmlns:ns2=\"http://pis.technologica.com/files/\">"

			"<ns1:user><ns1:egn>" + doctorEgn + "</ns1:egn></ns1:user>"

			"<ns1:file>"
				"<ns2:base64>" + Base64Convert::encode(data.data(), data.size()) + "</ns2:base64>"
				"<ns2:type>xml</ns2:type>"
				"<ns2:encoding>utf-8</ns2:encoding>"
			"</ns1:file>"

			"<ns1:purpose>AMB_DENT</ns1:purpose>"

		"</ns1:userFile>"
		;

	PisService::sendRequest(query, SOAPAction::Files);
}


void SendFileService::parseReply(const std::string& reply)
{

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	if (!doc.RootElement()) {
		u8"Неуспешно прочитане на отговора от ПИС";
	}

	TiXmlHandle docHandle(&doc);

	//error check

	auto error =
		docHandle
		.FirstChildElement()			  //envelope
		.FirstChildElement()			  //body (in case of error, header is missing)
		.FirstChildElement()			  //Fault
		.Child(1).ToElement();			  //faultstring

	if (error && error->ValueStr() == "faultstring") {
		ModalDialogBuilder::showError(error->GetText());
		return;
	}

	auto success =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement().ToElement(); //userFieldId

	if (success && success->GetText()) {
		ModalDialogBuilder::showMessage(
			u8"Файлът е изпратен успешно!\n"
			u8"Следете статуса му на обработка от Подадени файлове"
		);
	}


}