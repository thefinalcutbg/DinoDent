#pragma once

#include "AbstractReplyHandler.h"
#include <functional>
#include <map>
#include <string>
#include <vector>

class BZSRegisterService final : protected AbstractReplyHandler
{
public:
    BZSRegisterService() = default;
    BZSRegisterService(const BZSRegisterService&) = delete;
    BZSRegisterService& operator=(const BZSRegisterService&) = delete;

    bool sendRequest(const std::vector<std::string>& lpks,
        std::function<void(const std::map<std::string, std::string>&)> callback);

    bool awaitingReply() { return awaiting_reply; }

private:
    void parseReply(const std::string& reply) override;
    void queryNext();
    std::vector<std::string> m_lpks;
    std::size_t m_next = 0;
    std::map<std::string, std::string> m_results;
    std::function<void(const std::map<std::string, std::string>&)> m_callback;
};
