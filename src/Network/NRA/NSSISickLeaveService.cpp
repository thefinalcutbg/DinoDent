#include "NSSISickLeaveService.h"

#include <TinyXML/tinyxml.h>

#include "View/ModalDialogBuilder.h"
#include "Network/GetHSM.h"
#include "Network/NetworkManager.h"
#include "Model/User.h"
#include "Model/FreeFunctions.h"
#include "Database/DbPatient.h"


std::string NSSISickLeaveService::tag(const std::string& name, const std::string& value)
{
    std::string escaped;

    for (char c : value) {
        switch (c) {
        case '&': escaped += "&amp;"; break;
        case '<': escaped += "&lt;"; break;
        case '>': escaped += "&gt;"; break;
        default:  escaped += c; break;
        }
    }

    return "<" + name + ">" + escaped + "</" + name + ">";
}

std::string NSSISickLeaveService::tag(const std::string& name, long long value)
{
    return tag(name, std::to_string(value));
}

std::string  NSSISickLeaveService::regimenXml(int regimeType, int days)
{
    if (regimeType < 1 || regimeType > 7 || days < 1 || days > 999) {
        return "Невалиден режим или брой дни";
    }

    std::string hospital, home, sanatorium, unrestricted;
    const auto count = std::to_string(days);

    switch (regimeType) {
    case 1:
        hospital = count;
        break;
    case 2: case 3: case 4:
        home = count;
        break;
    case 5:
        sanatorium = count;
        break;
    case 6: case 7:
        unrestricted = count;
        break;
    }

    std::string xml = tag("RegimenDaysSubTypes", regimeType);
    xml += "<RegimenDays>";
    xml += tag("HospitalRegimenDays", hospital);
    xml += tag("HomeRegimenDays", home);
    xml += tag("SanatoriumRegimenDays", sanatorium);
    xml += tag("UnrestrictedRegimenDays", unrestricted);
    xml += "</RegimenDays>";

    return xml;
}

bool NSSISickLeaveService::sendRequest(const Patient& patient, const SickLeave& sickLeave, const std::string& nrn, const std::string& deleteReason, decltype(m_callback) callback)
{
	if (awaiting_reply) return true;

	auto hsm = GetHSM::get(show_dialogs);

	if (!hsm) { return false; }

    m_callback = callback;

    auto currentDate = Date::currentDate();

    std::string query =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
        "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"
        "<soap:Body>"
        "<ePCLoadSaveXMLNew xmlns=\"https://eBenefits.nssi.bg/ePCWSUploadDataOnline\">"
        "<inXML><![CDATA[<root>";

    query.reserve(5000);

    auto& practice = User::practice();
    auto& doctor = User::doctor();
    auto recipient = sickLeave.caregiver_rowid ? DbPatient::get(sickLeave.caregiver_rowid) : patient;

    auto& q = query;

    
    q += ("<RowNumber>");
    q += tag("Number", "1");
    q += tag("PatientsChartNumber", "E" + std::to_string(sickLeave.number));
    q += tag("CodeOperation", static_cast<bool>(deleteReason.size()));
    q += tag("PatientsChartType", sickLeave.isContinuation);
    q += "<NumberOfMedicalCard/>";
    q += tag("NumberOfMedicalExam", nrn);
    q += "<NumberInHospital/>";
    q += "<PCIssuedBy>";

    q += "<MedicalInstitutionType/>";
    q += tag("MedicalInstitutionName", practice.name);
    q += tag("MedicalInstitutionRegistrationNumber", practice.rziCode);

    q += "<DoctorNames>";
    q += tag("FirstNameDoctor", doctor.fname);
    q += tag("FamilyNameDoctor", doctor.lname);
    q += "</DoctorNames>";

    q += "<LKKTypeCode/><LKKTypeText/><HospitalDivision/>";

    q += "<MedicalInstitutionFullAddress>";
    q += tag("MedicalInstitutionAddressObl", practice.practice_address.getRegionName());
    q += tag("MedicalInstitutionAddressCity", practice.practice_address.getString());
    q += tag("MedicalInstitutionAddress", practice.street_address);
    q += "</MedicalInstitutionFullAddress>";

    q += "</PCIssuedBy>";

        q += "<Insured>";
            q += "<InsuredNames>";
            q += tag("FirstName", recipient.FirstName);
            q += tag("SurName", recipient.MiddleName);
            q += tag("FamilyName", recipient.LastName);
            q += "</InsuredNames>";

        q += tag("EGN", recipient.id);
        q += tag("FlagEGN", recipient.type == Patient::EGN ? 0 : 2);
        q += tag("EGNAge", recipient.getAge(sickLeave.date));
        q += tag("Sex", recipient.sex);

        q += "<InsuredAddress>";
            q += tag("PatientsAddressObl", recipient.city.getRegionCode());
            q += tag("PatientsAddressCity", recipient.city.getString());
            q += tag("PatientsAddress", recipient.address);
        q += "</InsuredAddress>";
        q += "</Insured>";

        q += "<Insurer>";
            q += tag("InsurerName", sickLeave.employerName);
                q += "<InsurerAddressFull>";
                q += tag("InsurerAddressObl", sickLeave.employerCity.getRegionName());
                q += tag("InsurerAddressCity", sickLeave.employerCity.getString());
                q += "<InsurerAddress/>";
            q += "</InsurerAddressFull>";

            q += tag("Profession", sickLeave.profession);
            q += tag("Position", sickLeave.position);
        q += "</Insurer>";

        q += "<SickLeaveStartDate>";
            q += tag("SickLeaveStartDate_Day", sickLeave.startDate.day);
            q += tag("SickLeaveStartDate_Month", sickLeave.startDate.month);
            q += tag("SickLeaveStartDate_Year", sickLeave.startDate.year);
        q += "</SickLeaveStartDate>";

        q += "<SickLeaveEndDate>";
            q += tag("SickLeaveEndDate_Day", sickLeave.endDate.day);
            q += tag("SickLeaveEndDate_Month", sickLeave.endDate.month);
            q += tag("SickLeaveEndDate_Year", sickLeave.endDate.year);
        q += "</SickLeaveEndDate>";

        q += tag("SickLeaveLegalDays", sickLeave.daysCount());
        q += tag("SickLeaveLegalDaysWords", sickLeave.dayCountWords());

        q += "<Diagnosis>";
            q += tag("DiagnosisCode", sickLeave.diagnosis.code());
            q += tag("DiagnosisName", sickLeave.diagnosis.name());
        q += "</Diagnosis>";
        
        q += "<ReasonOfDisease>";
            q += tag("ReasonOfDiseaseCode", sickLeave.reasonCode);
            q += tag("ReasonOfDiseaseName", sickLeave.reasons.at(sickLeave.reasonCode));
        q += "</ReasonOfDisease>";

        q+= regimenXml(sickLeave.regimeType, sickLeave.daysCount());

        q+= +R"xml(
<PregnancyTermDate>
<PregnancyTermDate_Day/>
<PregnancyTermDate_Month/>
<PregnancyTermDate_Year/>
</PregnancyTermDate>

<PregnancyDeliveryDate>
<PregnancyDeliveryDate_Day/>
<PregnancyDeliveryDate_Month/>
<PregnancyDeliveryDate_Year/>
</PregnancyDeliveryDate>
)xml";

        if (sickLeave.caregiver_rowid) {
            q += "<SickFamilyMember>";
            q += tag("TypeSickCode", sickLeave.relationship);
            q += tag("TypeSickName", sickLeave.relationships.at(sickLeave.relationship));
            q += "<SickFamilyMemberNames>";
            q += tag("NamesSick_FirstName", patient.FirstName);
            q += tag("NamesSick_SurName", patient.MiddleName);
            q += tag("NamesSick_FamilyName", patient.LastName);
            q += "</SickFamilyMemberNames>";
            q += tag("EGNSick", patient.id);
            q += tag("FlagEGNSick", patient.type == Patient::EGN ? 0 : 2);
            q += "</SickFamilyMember>";

            q += "<DateMedicalCheck>";
            q += tag("DateMedicalCheckDay", sickLeave.checkup.day);
            q += tag("DateMedicalCheckMonth", sickLeave.checkup.month);
            q += tag("DateMedicalCheckYear", sickLeave.checkup.year);
            q += "</DateMedicalCheck>";
        }
        else {
                   q+= +R"xml(
<SickFamilyMember>
<TypeSickCode/>
<TypeSickName/>

<SickFamilyMemberNames>
    <NamesSick_FirstName/>
    <NamesSick_SurName/>
    <NamesSick_FamilyName/>
</SickFamilyMemberNames>

<EGNSick/>
<FlagEGNSick/>
</SickFamilyMember>
)xml";
        }


        q += R"xml(
<LKKDecision>
    <LKKDecisionNumber/>

    <LKKDecisionDate>
    <LKKDecisionDateDay/>
    <LKKDecisionDateMonth/>
    <LKKDecisionDateYear/>
    </LKKDecisionDate>
</LKKDecision>

<TELKDecision>
    <TELKDecisionNumber/>

    <TELKDecisionDate>
    <TELKDecisionDateDay/>
    <TELKDecisionDateMonth/>
    <TELKDecisionDateYear/>
    </TELKDecisionDate>
</TELKDecision>
)xml";

            q += "<Notes>";
            q += tag("CodeNote", sickLeave.noteCode ? std::to_string(sickLeave.noteCode) : "");
            q += tag("TextNote", sickLeave.noteCode ? sickLeave.notes.at(sickLeave.noteCode) : "");
            q += tag("AdditionalNote", sickLeave.additionalNote);
            q += "</Notes>";

            q += "<IssuedBy>";
            q += "<PatientsChartIssuedBy>1</PatientsChartIssuedBy>";
            q += tag("FamilyNameDoctor1", doctor.lname);
            q += tag("UIN1", "0" + doctor.LPK);
            q += "<FamilyNameDoctor2/><UIN2/><FamilyNameDoctor3/><UIN3/>";
            q += "</IssuedBy>";

            q += "<DateIssued>";
            q += tag("DateIssuedDay", sickLeave.date.day);
            q += tag("DateIssuedMonth", sickLeave.date.month);
            q += tag("DateIssuedYear", sickLeave.date.year);
            q += "</DateIssued>";

            q += "<Deleted>";
            q += tag("DeletedPCNumber", deleteReason.size() ? "E" + std::to_string(sickLeave.number) : "");
            q += tag("DeletedRegMedInsNumber", deleteReason.size() ? practice.rziCode : "");
            q += tag("ReasonForDelete", deleteReason); //it's empty either
            q += tag("ReasonForDeleteCode", deleteReason.size() ? "2" : "")
            + "</Deleted>";
                 
            q += tag("DateSaved", currentDate.to8601());
            q += "<TimeSaved/>";
                
            q += tag("ClientBULSTAT", practice.bulstat);
            q += tag("ClientMedInsRegNumber", practice.rziCode);
            q += tag("ClientEGN", hsm->getDetails().serialNumber.substr(6));
            q += tag("ClientUIN", "0" + doctor.LPK);
            q += "<Exported>2</Exported>";

            q += "<DateExport>";
            q += tag("DateExportDay", currentDate.day);
            q += tag("DateExportMonth", currentDate.month);
            q += tag("DateExportYear", currentDate.year);
            q += "</DateExport>";

            q += tag("Source", User::practice().bulstat);
            q += "</RowNumber>";
            
            q += R"xml(
</root>]]></inXML>
    </ePCLoadSaveXMLNew>
  </soap:Body>
</soap:Envelope>
)xml";

    const std::string serviceUrl =  "https://ebenefits.nssi.bg/ePCWSUploadDataOnline/ePCUploadDataOnline.asmx";

    const std::string soapAction = "https://eBenefits.nssi.bg/ePCWSUploadDataOnline/ePCLoadSaveXMLNew";

    awaiting_reply = true;

    NetworkManager::sendRequestToNssiSickLeave(
        query,
        *hsm,
        this,
        serviceUrl,
        soapAction
    );

    return true;
}

std::string elementText(TiXmlElement* parent, const char* name)
{
    TiXmlElement* element = parent->FirstChildElement(name);
    const char* text = element ? element->GetText() : nullptr;
    return text ? text : "";
}

static void appendXmlText(const TiXmlNode* node, std::string& output)
{
    if (!node) return;

    if (const TiXmlText* text = node->ToText()) {
        const std::string value = text->Value();

        if (value.find_first_not_of(" \t\r\n") != std::string::npos) {
            if (!output.empty()) output += '\n';
            output += value;
        }
        return;
    }

    for (const TiXmlNode* child = node->FirstChild();
         child;
         child = child->NextSibling()) {
        appendXmlText(child, output);
    }
}

static std::string sectionText(TiXmlElement* parent, const char* name)
{
    std::string text;
    if (parent) {
        appendXmlText(parent->FirstChildElement(name), text);
    }
    return text;
}

void NSSISickLeaveService::parseReply(const std::string& reply)
{
    auto cb = std::move(m_callback);

    const auto showReadError = []() {
        ModalDialogBuilder::showMessage(
            "Грешка при прочитане на отговора от НОИ.");
    };

    TiXmlDocument soap;
    soap.Parse(reply.c_str(), nullptr, TIXML_ENCODING_UTF8);

    if (soap.Error()) { showReadError(); return; }

    TiXmlElement* envelope = soap.RootElement();
    TiXmlElement* body = envelope ? envelope->FirstChildElement("soap:Body") : nullptr;
    TiXmlElement* fault = body ? body->FirstChildElement("soap:Fault") : nullptr;

    if (fault) {
        std::string message = elementText(fault, "faultstring");
        std::string detail = sectionText(fault, "detail");

        if (message.empty()) {
            message = "Грешка при обработка на заявката от НОИ.";
        }

        if (!detail.empty()) {
            message += "\n\n" + detail;
        }

        ModalDialogBuilder::showError(message);
        return;
    }

    TiXmlElement* response = body ? body->FirstChildElement("ePCLoadSaveXMLNewResponse") : nullptr;
    TiXmlElement* payload = response ? response->FirstChildElement("ePCLoadSaveXMLNewResult") : nullptr;

    if (!payload) { showReadError(); return; }

    TiXmlDocument inner;
    TiXmlElement* root = payload->FirstChildElement("root");

    // If there is no nested root, parse the escaped XML text.
    if (!root) {
        const char* xmlText = payload->GetText();

        if (!xmlText) { showReadError(); return; }

        inner.Parse(xmlText, nullptr, TIXML_ENCODING_UTF8);

        if (inner.Error()) { showReadError(); return; }

        root = inner.RootElement();
    }

    TiXmlElement* result = root ? root->FirstChildElement("result") : nullptr;

    if (!result) { showReadError(); return; }

    std::string kepError = elementText(result, "KEPCheck");

    if (kepError.find_first_not_of(" \t\r\n") != std::string::npos) {
        ModalDialogBuilder::showError(kepError);
        return;
    }

    std::string systemError = elementText(result, "SysError");

    if (systemError.find_first_not_of(" \t\r\n") != std::string::npos) {
        ModalDialogBuilder::showError(systemError);
        return;
    }

    std::string errorMessage;

    const auto collectErrors = [&](const char* sectionName) {
        TiXmlElement* rejected = result->FirstChildElement(sectionName);

        for (TiXmlElement* pc = rejected
                 ? rejected->FirstChildElement("PC") : nullptr;
             pc;
             pc = pc->NextSiblingElement("PC"))
        {
            TiXmlElement* errors = pc->FirstChildElement("Errors");
            bool cancellationHandled = false;

            for (TiXmlElement* error = errors
                     ? errors->FirstChildElement("Error") : nullptr;
                 error;
                 error = error->NextSiblingElement("Error"))
            {
                const std::string code = elementText(error, "ErrCode");

                if (std::string(sectionName) == "A_III_2_ErrDetails" &&
                    code == "1205")
                {
                    const std::string number =
                        elementText(pc, "PatientsChartNumber");

                    if (!cancellationHandled && cb && number.size() > 1) {
                        cb(std::stoll(number.substr(1)));
                        cancellationHandled = true;
                    }

                    continue;
                }

                const std::string text = elementText(error, "ErrText");

                if (!text.empty()) {
                    if (!errorMessage.empty()) errorMessage += "\n";
                    errorMessage += text;
                }
            }
        }
    };

    collectErrors("A_III_1_ErrDetails");
    collectErrors("A_III_2_ErrDetails");

    if (!errorMessage.empty()) {
        ModalDialogBuilder::showError(errorMessage);
    }

    collectErrors("A_III_1_ErrDetails"); // Error on submition
    collectErrors("A_III_2_ErrDetails"); // Error on cancellation

    if (!errorMessage.empty()) {
        ModalDialogBuilder::showError(errorMessage);
    }

    const auto notifyAccepted = [&](const char* sectionName,
                                    const char* detailsName) {
        TiXmlElement* accepted = result->FirstChildElement(sectionName);
        TiXmlElement* details = accepted
            ? accepted->FirstChildElement(detailsName) : nullptr;

        for (TiXmlElement* pc = details
                 ? details->FirstChildElement("PC") : nullptr;
             pc;
             pc = pc->NextSiblingElement("PC"))
                    {
            const std::string sickLeaveNumber =
                elementText(pc, "PatientsChartNumber");

            if (cb && sickLeaveNumber.size() > 1) {
                cb(std::stoll(sickLeaveNumber.substr(1)));
            }
        }
    };

    notifyAccepted("A_I_1", "A_I_1_Details");
    notifyAccepted("A_II_1", "A_II_1_Details");
}