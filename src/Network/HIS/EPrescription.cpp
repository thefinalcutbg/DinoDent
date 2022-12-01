#include "EPrescription.h"
#include "Model/Prescription/Prescription.h"
#include "View/ModalDialogBuilder.h"
#include "Model/FreeFunctions.h"
#include "TinyXML/tinyxml.h"

bool EPrescription::Issue::sendRequest(const Prescription& perscr, const Patient& patient, std::function<void(const std::string&)> nrnCallback)
{
	m_callback = nrnCallback;

	std::string contents;
	
	contents.reserve(4000);

	contents+=
		"<nhis:prescription>"
			+ bind("lrn", perscr.LRN)
		    + bind("authoredOn", perscr.date.to8601())
			+ bind("category", perscr.category)
			+ bind("isProtocolBased", false)
			+ bind("financingSource", "4")
			+ bind("dispensationType", perscr.dispensation.getNhisDispensation())
			+ bind("allowedRepeatsNumber", perscr.dispensation.getNhisRepeats())
			+ bind("supplements", perscr.supplements)
			+ "<nhis:group>"
	;

	//Medications:
	for(int i = 0; i < perscr.medicationGroup.size(); i++)
	{
		auto& m = perscr.medicationGroup[i];

		contents+=
				"<nhis:medication>"
					+ bind("sequenceId", i+1)
					+ "<nhis:medicationCode "
							"value=\"" + std::to_string(m.getNumenclatureKey()) + "\" " 
							"name=\"" + m.name() + "\" />"
						
					+ bind("form", m.getFormKey(), false)
					+ bind("priority", m.priority)
					+ bind("note", m.note)
					+ bind("quantityValue", static_cast<int>(m.quantity))
					+ bind("isQuantityByForm", m.byForm)
					+ bind("isSubstitutionAllowed", m.substitution)
			;
				//Dosages:
				for (int y = 0; y < m.dosage.size(); y++)
				{
					auto& d = m.dosage[y];

					contents +=
						"<nhis:dosageInstruction>"
							+ bind("sequence", y+1)
							+ bind("asNeeded", d.asNeeded)
							+ bind("route", d.route.getKey())
							+ bind("doseQuantityValue", d.doseQuantity.value)
							+ bind("doseQuantityCode", d.doseQuantity.getXmlUnitValue())
							+ bind("frequency", static_cast<int>(d.frequency))
							+ bind("period", d.period.value)
							+ bind("periodUnit", d.period.getNHISKey())
							+ bind("boundsDuration", d.bounds.value)
							+ bind("boundsDurationUnit", d.bounds.getNHISKey())
					;

							for (auto tagIdx : d.when.getTagIdx()) contents += bind("when", tagIdx);

							if (d.when.getTagIdx().size()) contents += bind("offset", d.when.getOffset());

							contents += bind("text", d.additionalInstructions);
							contents += bind("interpretation", d.parse());

					contents+= "</nhis:dosageInstruction>";

				}

		contents += "</nhis:medication>";
	}
					
	contents += "</nhis:group>"
				"</nhis:prescription>"
				+ subject(patient)
				+ requester();

	return HisService::sendRequestToHis(contents);
}


void EPrescription::Issue::parseReply(const std::string& reply)
{
	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto nrnPrescription = docHandle.
		FirstChild(). //message
		Child(1).	  //contents
		FirstChildElement().ToElement(); //nrnPrescription

	if (nrnPrescription) {
		m_callback(nrnPrescription->Attribute("value"));
	}
	else
	{
		m_callback(std::string());
	}
}



bool EPrescription::Cancel::sendRequest(const std::string& nrn, std::function<void(bool)> success)
{
	m_callback = success;

	auto reason = ModalDialogBuilder::inputDialog(
						"Основание за анулиране на рецептата:",
						"Анулиране на рецепта"
					);

	if (reason.empty()) return false;

	std::string contents = 
		bind("nrnPrescription", nrn)
	+	bind("revokeReason", reason);

	return HisService::sendRequestToHis(contents);
}

void EPrescription::Cancel::parseReply(const std::string& reply)
{
	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto status = docHandle.
		FirstChild(). //message
		Child(1).	  //contents
		Child(1).ToElement(); //status

	if (
		status &&
		status->FirstAttribute()->IntValue() == 5
	)
	{
		m_callback(true); return;
	}
	
	m_callback(false);

}
