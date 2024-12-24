#pragma once
#include "Model/Date.h"

struct Notification
{
    long long rowid {0};
    long long patientRowid {0};
    Date date;
    std::string description;
    std::string patientName;
    std::string phone;
};
