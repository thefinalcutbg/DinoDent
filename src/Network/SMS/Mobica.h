#pragma once

#include <string>
#include <vector>
#include "Network/AbstractReplyHandler.h"
#include "Network/SMS/SmsMessage.h"

class AbstractReplyHandler;

namespace Mobica
{
    class SmsReplyHandler : protected AbstractReplyHandler
    {
       const std::string url = "https://gate.mobica.bg/v2/multi/json/sms.php";
    protected:
        void parseReply(const std::string& reply) override;
    public:
        void sendSms(const std::vector<SmsMessage>& messages);
    };

    class BalanceReplyHandler : protected AbstractReplyHandler
    {
        const std::string url = "https://gate.mobica.bg/v2/user/account_balance.php";
    protected:
        void parseReply(const std::string& reply) override;
    public:
        void checkBalance(const std::string& usr, const std::string& pass);
    };



}
