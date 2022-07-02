#include "FileSentHandler.h"

#include <TinyXML/tinyxml.h>

#include "QDebug"
#include "View/ModalDialogBuilder.h"

void FileSentHandler::getReply(const std::string& reply)
{
	if (reply.empty()) {
		ModalDialogBuilder::showError(u8"Неуспешна връзка със сървъра");
		return;
	}
	
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
			u8"Следете статуса му на обработка от сайта на ПИС"
		);
	}

	
}
