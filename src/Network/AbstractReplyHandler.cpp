#include "AbstractReplyHandler.h"
#include "TinyXML/tinyxml.h"
#include "QDebug"
#include "View/ModalDialogBuilder.h"
#include "NetworkManager.h"

std::string AbstractReplyHandler::getTextNullCheck(const TiXmlElement* e)
{
    auto ptr = e->GetText();

        if ( ptr == nullptr) {
            return "";
        }

        return ptr;

}

void AbstractReplyHandler::getReply(const std::string& reply)
{
	awaiting_reply = false;

	NetworkManager::unsubscribeHandler(this);

	if (reply.empty()) {
		ModalDialogBuilder::showError("Неуспешна връзка със сървъра");
	}

	parseReply(reply);
}

AbstractReplyHandler::~AbstractReplyHandler() {
	NetworkManager::unsubscribeHandler(this);
}

