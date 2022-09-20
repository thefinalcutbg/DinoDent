#include "DiagnosisService.h"

#include <TinyXML/tinyxml.h>

#include "Model/Dental/ToothUtils.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/Date.h"
#include "Model/Dental/MKB.h"

bool DiagnosisService::sendRequest(int personType, const std::string& patientId, std::function<void(const std::string&)> callback)
{
	m_callback = callback;

	std::string tag = personTypeArr[personType];

	auto query =
		"<ns3:pdDiagnosisIn xmlns:ns3 = \"http://pis.technologica.com/ws/\">"
			"<ns3:" + tag + ">" + patientId + "</ns3:" + tag + ">"
			"<ns3:date>" + Date::currentDate().to8601()  +"</ns3:date>"
		"</ns3:pdDiagnosisIn>"
	;

	return PisService::sendRequest(query, SOAPAction::Diagnosis);

}

void DiagnosisService::parseReply(const std::string& reply)
{

	//ModalDialogBuilder::showMultilineDialog(reply);

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	if (!doc.RootElement()) { return; }

	TiXmlHandle docHandle(&doc);

	auto table =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement();			  //diagnosisOut


	std::string result;

	//i is 1, since 0 is the egn
	for (int i = 1; ; i++)
	{
		auto mkb = table.Child(i)	//pbd
						.Child(1);	//icd10Code

		if (!mkb.ToElement()) {
			break;
		}

		result.append(
			MKB::getNameFromMKBCode(
				mkb.ToElement()->GetText()
			) + ". "
		);
	}

	m_callback({result});

}
