#include "AbstractReplyHandler.h"
#include "TinyXML/tinyxml.h"
#include "QDebug"
#include "View/ModalDialogBuilder.h"
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

	if (reply.empty()) {
		ModalDialogBuilder::showError(u8"Неуспешна връзка със сървъра");
	}

	parseReply(reply);
}
