#include "MonthNotifDataHandler.h"
#include "TinyXML/tinyxml.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/MonthNotifLoader.h"
#include "../Base64Convert.h"
MonthNotifDataHandler::MonthNotifDataHandler(MonthNotifLoader* l) :
loader{l}
{
}

void MonthNotifDataHandler::getReply(const std::string& reply)
{
	if (loader == nullptr) return;

	if (reply.empty()) {
		ModalDialogBuilder::showError(u8"Неуспешна връзка със сървъра");
		loader->setMonthNotif("");
		return;
	}


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
		loader->setMonthNotif(Base64Convert::decode(getTextNullCheck(notifData)));
	}
	else
	{
		loader->setMonthNotif("");
	}
}
