#include "FileSentHandler.h"
#include "QDebug"
#include "View/ModalDialogBuilder.h"

void FileSentHandler::getReply(const std::string& reply)
{

	qDebug() << reply.data();
	ModalDialogBuilder::showErrorList(reply);
}
