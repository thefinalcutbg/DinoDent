#pragma once
#include "Model/Dental/Notification.h"

namespace DbNotification
{
    bool insert(const Notification& n);
    std::vector<Notification> get(const Date& currentDate, const std::string& lpk);

}
