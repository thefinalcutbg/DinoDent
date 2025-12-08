#include "Mobica.h"

#include "Network/NetworkManager.h"
#include "View/ModalDialogBuilder.h"
#include "Model/User.h"

#include <json/json.h>
#include <QString>

using namespace Mobica;

static std::string normalizePhoneForMobica(const std::string& raw)
{
    //Keep only digits
    QString digits;

    for (QChar c : QString::fromStdString(raw)) {
        if (c.isDigit()) {
            digits.append(c);
        }
    }

    if (digits.isEmpty()) {
        return std::string();
    }

    //Remove leading 00
    if (digits.startsWith("00")) {
        digits.remove(0, 2);
    }

    if (digits.startsWith("359")) {
        return digits.toStdString();
    }

    // drop the 0 and prepend 359
    if (digits.startsWith('0')) {
        digits.remove(0, 1);
        digits.prepend("359");
        return digits.toStdString();
    }

    //missing leading 0
    if (digits.startsWith('8')) {
        digits.prepend("359");
        return digits.toStdString();
    }

    //if still not with 359, prepend it.
    if (!digits.startsWith("359")) {
        digits.prepend("359");
    }

    return digits.toStdString();
}

static std::string translateCodeToBg(const std::string& code)
{
    int codeInt = 0;

    try {
        codeInt = std::stoi(code);
    }
    catch (...) {
        return "Неочакван отговор от Mobica.";
    }

    switch (codeInt) {
    case 1004: return "SМS съобщението е прието за изпращане.";
    case 1005: return "SMS заявката е отхвърлена";
    case 1006: return "Невалиден телефонен номер.";
    case 1007: return "Невалидни данни за отложено изпращане.";
    case 1008: return "Невалиден формат на заявката.";
    case 1115: return "Невалидно потребителско име или парола.";
    case 1117: return "Недостатъчна наличност по акаунта.";
    case 1120: return "Липсва стойност за задължително поле.";
    case 1121: return "Заявката е изпратена от неразрешен IP адрес.";
    case 1122: return "Акаунтът не е активен.";
    default:
        return "Неочакван отговор от Mobica.";
    }
}

void Mobica::SmsReplyHandler::sendSms(const std::vector<SmsMessage>& messages)
{
    if (messages.empty()) return;

    Json::Value json;

    json["user"] = User::practice().settings.sms_settings.usr;
    json["pass"] = User::practice().settings.sms_settings.pass;

    Json::Value smsArray(Json::arrayValue);

    for (const auto& msg : messages) {
        Json::Value sms;
        sms["phone"] = normalizePhoneForMobica(msg.phone);
        sms["message"] = msg.message;

        if (!msg.idd.empty()) {
            sms["idd"] = msg.idd;
        }

        if (!msg.toDate.empty()) {
            sms["toDate"] = msg.toDate;
        }

        smsArray.append(sms);
    }

    json["sms"] = smsArray;

    Json::FastWriter writer;

    NetworkManager::sendRequestToMobicaSms(writer.write(json), url, this);
}

void SmsReplyHandler::parseReply(const std::string& reply)
{
    if (reply.empty()) {
        return;
    }

    Json::Value json;
    Json::Reader reader;

    if (!reader.parse(reply, json)) {
        ModalDialogBuilder::showError("Невалиден JSON отговор от Mobica.");
        return;
    }

    const std::string code = json.isMember("code") ? json["code"].asString() : std::string{};

    if (code.size()) {
        ModalDialogBuilder::showMessage(translateCodeToBg(code));
    }

}

void BalanceReplyHandler::parseReply(const std::string& reply)
{
    if (reply.empty()) return;

    Json::Value json;
    Json::Reader reader;

    if (!reader.parse(reply, json)) {
        ModalDialogBuilder::showError("Невалиден отговор от Mobica.");
        return;
    }

    std::string balanceStr;

    if (json.isMember("amount")) {
        const Json::Value& b = json["amount"];
            balanceStr = b.asString();
    }

    std::string currency;
    if (json.isMember("currency") && json["currency"].isString()) {
        currency = json["currency"].asString();
    }

    if (!balanceStr.empty()) {
        std::string msg = "Текущ баланс: " + balanceStr + currency;
        ModalDialogBuilder::showMessage(msg);
        return;
    }

    if (json.isMember("code")) {
        ModalDialogBuilder::showError(translateCodeToBg(json["code"].asString()));
        return;
    }

    ModalDialogBuilder::showError("Неочакван формат на отговора за баланс от Mobica.");
}

void Mobica::BalanceReplyHandler::checkBalance(const std::string& usr, const std::string& pass)
{
    Json::Value root;
    root["user"] = usr;
    root["pass"] = pass;

    Json::FastWriter writer;
    const std::string jsonRequest = writer.write(root);

    NetworkManager::sendRequestToMobicaSms(jsonRequest, url , this);
}
