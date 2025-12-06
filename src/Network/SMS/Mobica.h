#pragma once

#include <string>
#include <vector>
#include "Network/AbstractReplyHandler.h"

class AbstractReplyHandler;

namespace Mobica
{

    struct SmsMessage
    {
        std::string phone;
        std::string message;
        std::string idd;
        std::string toDate;
    };

    class SmsReplyHandler : public AbstractReplyHandler
    {
       const std::string url = "https://gate.mobica.bg/v2/multi/json/sms.php";
    protected:
        void parseReply(const std::string& reply) override;
    public:
        void sendSms(const std::vector<SmsMessage>& messages);
    };

    class BalanceReplyHandler : public AbstractReplyHandler
    {
        const std::string url = "https://gate.mobica.bg/v2/user/account_balance.php";
    protected:
        void parseReply(const std::string& reply) override;
    public:
        void checkBalance(const std::string& usr, const std::string& pass);
    };



}
