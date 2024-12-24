#pragma once
#include "Model/Dental/Notification.h"

namespace DbNotification
{
    bool insert(const Notification& n);
    bool remove(long long notifRowid);
    std::vector<Notification> get(const Date& currentDate, const std::string& lpk);
    int hasNotifications(const Date& date = Date::currentDate());

}
