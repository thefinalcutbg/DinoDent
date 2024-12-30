#pragma once

#include "HisService.h"
#include <functional>
#include "Model/ICD10.h"
#include "Model/Patient.h"

namespace EClinicalCondition
{

class Fetch : private HisService
{
    std::function<void(const std::vector<ICD10>& activeConditions, const std::vector<ICD10>& pastConditions)> m_callback;

protected:
    void parseReply(const std::string& reply) override;

public:
    Fetch() :
        HisService("L005", "/v1/longterm-care/doctor/clinical-condition-fetch") {}
    bool sendRequest(const Patient& patient, const std::string& rzi, decltype(m_callback) callback);
    bool awaitingReply() { return awaiting_reply; }

};

}
