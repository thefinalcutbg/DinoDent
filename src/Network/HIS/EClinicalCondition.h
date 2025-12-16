#pragma once

#include "HisService.h"
#include <functional>
#include "Model/MedicalStatuses.h"
#include "Model/Patient.h"

namespace EClinicalCondition
{

class Fetch : private HisService
{
    std::function<void(const std::vector<MedicalStatus>& activeConditions, const std::vector<MedicalStatus>& pastConditions)> m_callback;

protected:
    void parseReply(const std::string& reply) override;

public:
    Fetch() :
        HisService("L005", "/v1/longterm-care/doctor/clinical-condition-fetch") {}
    bool sendRequest(const Patient& patient, const std::string& rzi, decltype(m_callback) callback);
    bool awaitingReply() { return awaiting_reply; }
    using HisService::show_dialogs;

};

}
