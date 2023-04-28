#include "EReferral.h"

#include <TinyXML/tinyxml.h>

#include "Model/Patient.h"
#include "View/ModalDialogBuilder.h"

bool EReferral::Issue::sendRequest(const std::string& examNrn, const Patient& patient, const Referral& ref, std::function<void(const std::string&)> nrnCallback)
{
	m_callback = nrnCallback;

	std::string contents;

	auto getNhifCategory = [&]() {

		switch (ref.type)
		{
			case ReferralType::MDD4: return "R1";
			case ReferralType::Form3: return "R2";
			case ReferralType::Form3A: return "R3";
			default: return "";
		}

	};
		

	contents += "<nhis:referral>";
	contents += bind("lrn", ref.lrn);
	contents += bind("authoredOn", ref.date.to8601());
	contents += bind("category", getNhifCategory());
	contents += bind("type", ref.reason.getIndex());
	contents += bind("rhifAreaNumber", patient.city.getRhif() + patient.city.getHealthRegion());
	contents += bind("basedOn", examNrn);
	contents += bind("financingSource", 2);

	switch(ref.type)
	{
		case ReferralType::MDD4:
		{
			contents += "<nhis:laboratory>";
			auto ref4Data = std::get<MDD4Data>(ref.data);
			contents += bind("code", ref4Data.getHisCode());
			contents += "</nhis:laboratory>";
		}
			break;
		case ReferralType::Form3:
		{
			auto r3data = std::get<R3Data>(ref.data);
			contents += "<nhis:consultation><nhis:qualification value=\"";
			contents += r3data.hisSpecialty;
			contents += "\"  nhifCode = \"";
			contents += r3data.specialty;
			contents += "\"/></nhis:consultation>"
				;
		}
			break;
		case ReferralType::Form3A:
		{
			auto r3Adata = std::get<R3AData>(ref.data);
			contents += "<nhis:specializedActivities><nhis:qualification value=\"";
			contents += r3Adata.hisSpecialty;
			contents += "\" nhifCode=\"";
			contents += r3Adata.nhifSpecialty;
			contents += "\"/><nhis:code value=\"";
			contents += r3Adata.highlySpecializedActivity;
			contents += "\"/></nhis:specializedActivities>"
				;
		}
			break;
	}

	
	contents += "<nhis:diagnosis>";
	contents += bind("code", ref.diagnosis.main.code());
	contents += bind("additionalCode", ref.diagnosis.additional.code());
	contents += bind("use", 3);
	contents += bind("rank", 1);

	if (ref.type == ReferralType::MDD4)
	{
		auto& data = std::get<MDD4Data>(ref.data);

		if (!data.isOPG())
		{
			contents += bind("note", "Зъб " + ToothUtils::getToothNumber(data.tooth_idx, false));
		}
	}

	contents += "</nhis:diagnosis>";

	if (ref.comorbidity.main.isValid())
	{
		contents += "<nhis:comorbidity>";
		contents += bind("code", ref.comorbidity.main.code());
		contents += bind("additionalCode", ref.comorbidity.additional.code());
		contents += bind("use", 4);
		contents += bind("rank", 1);
		contents += "</nhis:comorbidity>";
	}

	contents += "</nhis:referral>";

	contents += subject(patient);
	contents += requester(true);

	return sendRequestToHis(contents);
}

void EReferral::Issue::parseReply(const std::string& reply)
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

bool EReferral::Cancel::sendRequest(const std::string& nrn, std::function<void(bool)> success)
{
	m_callback = success;

	auto reason = ModalDialogBuilder::inputDialog(
		"Основание за анулиране на направлението:",
		"Анулиране на направление"
	);

	if (reason.empty()) return false;

	std::string contents =
		bind("nrnReferral", nrn)
		+ bind("cancelReason", reason);

	return HisService::sendRequestToHis(contents);
}

void EReferral::Cancel::parseReply(const std::string& reply)
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
		status->FirstAttribute()->ValueStr() == "4"
		)
	{
		m_callback(true); return;
	}

	m_callback(false);

}