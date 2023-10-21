#include "EPrescription.h"
#include "Model/Prescription/Prescription.h"
#include "View/ModalDialogBuilder.h"
#include "Model/FreeFunctions.h"
#include "TinyXML/tinyxml.h"
#include "EReferral.h"

#include <algorithm>

bool EPrescription::Issue::sendRequest(const Prescription& prescr, const Patient& patient, std::function<void(const std::string&)> nrnCallback)
{
	m_callback = nrnCallback;

	std::string contents;
	
	contents.reserve(4000);

	contents+=
		"<nhis:prescription>"
			+ bind("lrn", prescr.LRN)
		    + bind("authoredOn", prescr.date.to8601())
			+ bind("category", prescr.category)
			+ bind("isProtocolBased", false)
			+ bind("financingSource", "4")
			+ bind("dispensationType", prescr.dispensation.getNhisDispensation())
			+ bind("allowedRepeatsNumber", prescr.dispensation.getNhisRepeats())
			+ bind("supplements", prescr.supplements, true)
			+ "<nhis:group>"
	;

	//Medications:
	for(int i = 0; i < prescr.medicationGroup.size(); i++)
	{
		auto& m = prescr.medicationGroup[i];

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

				//Effective Dose Period:

				if (m.dosePeriod)
				{
					contents += "<nhis:effectiveDosePeriod>";
					contents += bind("start", m.dosePeriod->from.to8601());
					contents += bind("end", m.dosePeriod->to.to8601());
					contents += "</nhis:effectiveDosePeriod>";
				}

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

							contents += bind("text", d.additionalInstructions, true);
							contents += bind("interpretation", d.parse());

					contents+= "</nhis:dosageInstruction>";

				}

		contents += "</nhis:medication>";
	}
					
	contents += "</nhis:group>"
				"</nhis:prescription>"
				+ subject(patient, prescr.isPregnant, prescr.isBreastFeeding)
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

bool EPrescription::Cancel::sendRequest(const std::string& nrn, std::function<void(bool)> callback)
{
	m_callback = callback;

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

	bool success = status && status->FirstAttribute()->IntValue() == 5;

	m_callback(success);

	m_callback = nullptr;

}

bool EPrescription::FetchDispense::sendRequest(const std::string& nrn, std::function<void(const std::string& response)> callback)
{
	m_callback = callback;

	std::string contents = bind("nrnPrescription", nrn);

	return HisService::sendRequestToHis(contents);
}

void EPrescription::FetchDispense::parseReply(const std::string& reply)
{
	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto statusXml = docHandle.
		FirstChild("nhis:message").
		FirstChild("nhis:contents").
		FirstChild("nhis:results").
		FirstChild("nhis:prescription").
		FirstChild("nhis:status").ToElement();

	int statusValue = 0;

	if (statusXml) {
		statusValue = statusXml->FirstAttribute()->IntValue();
	}


	static const char* statusNum[] = {
		"Неизвестен",
		"Активна",
		"Частично изпълнена",
		"Изпълнена",
		"Отказана",
		"Анулирана",
		"Частично анулирана",
		"Изтекла"
	};

	std::string result = "Статус на рецептата: ";
	result += statusNum[statusValue];

	auto substitutionXml = docHandle.
		FirstChild("nhis:message").
		FirstChild("nhis:contents").
		FirstChild("nhis:results").
		FirstChild("nhis:dispense").
		FirstChild("nhis:group").
		FirstChild("nhis:medication").
		FirstChild("nhis:substitution").ToElement();


	if (substitutionXml && getBool(substitutionXml, "wasSubstituted")){
		 result += "\nЗаместител: Да\nПричина за заместване: ";
		 result += getString(substitutionXml, "reason");
	}

	m_callback(result);

	m_callback = nullptr;

}




bool EPrescription::eRxFetch::sendRequest(
	const std::string& nrn,
	const std::string& lpk,
	const Patient& p,
	decltype(m_callback) callback
)
{
	m_callback = callback;

	std::string contents;
	contents += bind("nrnPrescription", nrn);
	contents += bind("pmi", lpk);
	contents += bind("identifierType", p.type);
	contents += bind("identifier", p.id);

	return HisService::sendRequestToHis(contents);
}

void EPrescription::eRxFetch::parseReply(const std::string& reply)
{
	auto errors = getErrors(reply);

	if (errors.size()) {
		ModalDialogBuilder::showError(errors);
		m_callback = nullptr;
		return;
	}

	TiXmlDocument doc;

	doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

	TiXmlHandle docHandle(&doc);

	auto prescrXml = docHandle.
		FirstChild().				//message
		Child(1).					//contents
		Child(1).					//results
		FirstChild().ToElement();	//prescription


	if (!prescrXml) {
		m_callback = nullptr;
		return;
	}

	if (getString(prescrXml, "category") != "T1") {
		ModalDialogBuilder::showMessage(
			"Не можете да зареждате рецепти различни от белите"
		);

		m_callback = nullptr;
		return;
	}

	Prescription result;

	result.NRN = getString(prescrXml, "nrnPrescription");
	result.LRN = getString(prescrXml,"lrn");
	result.date = getString(prescrXml,"authoredOn");
	result.supplements = getString(prescrXml, "nhis:supplements");
	result.dispensation.setTypeFromNhis(getInt(prescrXml,"dispensationType"));

	if (result.dispensation.type != Dispensation::SingleUse) {
		result.dispensation.setRepeatsFromNhis(getInt(prescrXml, "allowedRepeatsNumber"));
	}

	//Parsing medications
	for (
		auto medXml = prescrXml->FirstChildElement("nhis:group")->FirstChildElement("nhis:medication");
		medXml != nullptr;
		medXml = medXml->NextSiblingElement("nhis:medication")
		)
	{
		auto& medication = result.medicationGroup.emplace_back(getInt(medXml, "medicationCode"));
		medication.priority = static_cast<Medication::Priority>(getInt(medXml,"priority")-1);
		medication.quantity = getInt(medXml, "quantityValue");
		medication.byForm = getBool(medXml, "isQuantityByForm");
		medication.note = getString(medXml, "note");
		medication.substitution = getBool(medXml, "isSubstitutionAllowed");

		if (medXml->FirstChildElement("nhis:effectiveDosePeriod")) {
			medication.dosePeriod = {
				getString(medXml->FirstChildElement("nhis:effectiveDosePeriod"), "start"),
				getString(medXml->FirstChildElement("nhis:effectiveDosePeriod"), "end"),
			};
		}

		//Parsing dosage
		for (
			auto dosageXml = medXml->FirstChildElement("nhis:dosageInstruction");
			dosageXml != nullptr;
			dosageXml = dosageXml->NextSiblingElement("nhis:dosageInstruction")
		)
		{
			auto& dosage = medication.dosage.emplace_back();

			dosage.asNeeded = getBool(dosageXml, "asNeeded");
			dosage.route = getInt(dosageXml, "route");
			dosage.additionalInstructions = getString(dosageXml, "text");

			//dose quantity polymorphism
			std::string doseQuantityStr = getString(dosageXml, "doseQuantityCode");

			bool isNumber = !doseQuantityStr.empty() &&
				std::find_if(
					doseQuantityStr.begin(),
					doseQuantityStr.end(),
					[](unsigned char c) { return !std::isdigit(c); }
			) == doseQuantityStr.end();

			if (isNumber) {
				dosage.doseQuantity = getInt(dosageXml, "doseQuantityCode");
			}
			else {
				dosage.doseQuantity = doseQuantityStr;
			}

			dosage.doseQuantity.value = getDouble(dosageXml, "doseQuantityValue");
	
			dosage.frequency = getInt(dosageXml,"frequency");
			if (!dosage.frequency) dosage.frequency = 1; //default value

			dosage.period = {
				getDouble(dosageXml, "period"),
				getString(dosageXml, "periodUnit")
			};

			dosage.bounds = {
				getDouble(dosageXml, "boundsDuration"),
				getString(dosageXml, "boundsDurationUnit")
			};

			//parsing when

			for (
				auto whenXml = dosageXml->FirstChildElement("nhis:when");
				whenXml != nullptr;
				whenXml = whenXml->NextSiblingElement("nhis:when")
				) {

				dosage.when.addTag(whenXml->FirstAttribute()->IntValue());
			}

			dosage.when.setOffset(getInt(dosageXml, "offset"));

		}

	}

	auto various = docHandle.
		FirstChild("nhis:message").
		FirstChild("nhis:contents").
		FirstChild("nhis:results").
		FirstChild("nhis:subject").
		FirstChild("nhis:various").ToElement();
	
	if (various) {
		result.isPregnant = getBool(various, "isPregnant");
		result.isBreastFeeding = getBool(various, "isBreastFeeding");
	}

	m_callback(result);
}

