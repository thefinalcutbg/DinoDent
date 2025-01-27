#include "EClinicalCondition.h"
#include "View/ModalDialogBuilder.h"
#include "include/TinyXML/tinyxml.h"

bool EClinicalCondition::Fetch::sendRequest(
    const Patient& patient,
    const std::string& rzi,
    decltype(m_callback) callback
)
{
    m_callback = callback;

    std::string contents;

    contents += bind("identifierType", patient.type);
    contents += bind("identifier", patient.id);
    contents += bind("practiceNumber", rzi);

    return HisService::sendRequestToHis(contents);
}


void EClinicalCondition::Fetch::parseReply(const std::string &reply)
{
    if (reply.empty()) {
        m_callback = nullptr;
        return;
    }

    auto errors = getErrors(reply);

    if (errors.size()) {
        ModalDialogBuilder::showError(errors);
        m_callback = nullptr;
        return;
    }

    TiXmlDocument doc;

    doc.Parse(reply.data(), 0, TIXML_ENCODING_UTF8);

    TiXmlHandle docHandle(&doc);

    auto resultXml = docHandle
                            .FirstChild("nhis:message")
                            .FirstChild("nhis:contents")
                            .FirstChild("nhis:results")
                            .ToElement();

    std::vector<MedicalStatus> activeConditions;
    std::vector<MedicalStatus> inactiveConditions;

    while (resultXml != nullptr)
    {
        auto status = MedicalStatus();


        status.diagnosis = resultXml->
                    FirstChild("nhis:clinicalCondition")->
                    FirstChild("nhis:diagnosis")->
                    FirstChildElement("nhis:code")->FirstAttribute()->ValueStr();

        status.nrn = resultXml->
            FirstChild("nhis:clinicalCondition")->
            FirstChildElement("nhis:nrnCondition")->FirstAttribute()->ValueStr();

        bool isActive = resultXml->
                        FirstChild("nhis:clinicalCondition")->
                        FirstChild("nhis:diagnosis")->
                        FirstChildElement("nhis:clinicalStatus")->FirstAttribute()->IntValue() < 20;



        isActive ?
            activeConditions.push_back(status)
            :
            inactiveConditions.push_back(status);


        resultXml = resultXml->NextSiblingElement("nhis:results");
    }

    m_callback(activeConditions, inactiveConditions);

    m_callback = nullptr;
}
