#include "DiagnosisService.h"

#include <TinyXML/tinyxml.h>
#include <set>

#include "Model/Dental/ToothUtils.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/Date.h"
#include "Model/ICD10.h"
#include "Model/Patient.h"

bool DiagnosisService::sendRequest(const Patient& p, decltype(m_callback) callback)
{
	if (!ICD10::isInitialized()) {
		ModalDialogBuilder::showMessage("Първо заредете МКБ номенклатурите от настройки");
		return false;
	}

	m_callback = callback;

	std::string tag = getPersonIdTag(p);

	auto query =
		"<ns3:pdDiagnosisIn xmlns:ns3 = \"http://pis.technologica.com/ws/\">"
			"<ns3:" + tag + ">" + p.id + "</ns3:" + tag + ">"
		//	"<ns3:date>" + Date::currentDate().to8601()  +"</ns3:date>"
		"</ns3:pdDiagnosisIn>"
	;

	return PisService::sendRequest(query, SOAPAction::Diagnosis);

}

void DiagnosisService::processPISReply(const std::string& reply)
{

	//ModalDialogBuilder::showMultilineDialog(reply);

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	if (!doc.RootElement()) { return; }

	TiXmlHandle docHandle(&doc);

	std::vector<ICD10> current;
	std::vector<ICD10> past;

	std::set<std::string> unique;

	for (
		auto diagnosis =
		docHandle
		.FirstChildElement()			  //envelope
		.Child(1)						  //body
		.FirstChildElement()			  //diagnosisOut
		.FirstChildElement("ns1:pbd")
		.ToElement();
		diagnosis != nullptr;
		diagnosis = diagnosis->NextSiblingElement("ns1:pbd")
		)
	{
		auto mkb = diagnosis->FirstChildElement("ns1:icd10Code")->GetText();

		if (unique.count(mkb)) continue;

		unique.insert(mkb);

		//if dateTo is missing, then the disease is active
		bool isPast = diagnosis->FirstChildElement("ns1:dateTo");

		std::vector<ICD10>* diseases = isPast ? &past : &current;
		
		auto diag = ICD10(mkb);

		if (!diag.isValid()) continue;

		diseases->push_back(diag);

	}

	m_callback(current, past);

}
