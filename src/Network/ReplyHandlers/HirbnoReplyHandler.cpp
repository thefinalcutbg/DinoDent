#include "HirbnoReplyHandler.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "Network/PISServ.h"
#include <TinyXML/tinyxml.h>
#include "Model/FreeFunctions.h"
#include <QDebug>
HirbnoReplyHandler::HirbnoReplyHandler(PatientDialogPresenter* presenter) :
	p_presenter{ presenter }
{
}

void HirbnoReplyHandler::getReply(const std::string& reply)
{
	if (p_presenter == nullptr) return;

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
		p_presenter->setHirbno("");
		return;
	}

	std::string result = hirbno->GetText();

	//PIS has hirb numbers leading zeroes all messed up, so we have to normalize them
	result.size() > 8 ? 
		result = result.substr(result.size() - 8)
		:
		result = leadZeroes(result, 8);

	p_presenter->setHirbno(result);

}
