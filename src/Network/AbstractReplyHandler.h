#pragma once

#include <string>
#include <memory>

class TiXmlElement;

class AbstractReplyHandler : public std::enable_shared_from_this<AbstractReplyHandler>
{
public:
    virtual ~AbstractReplyHandler();

protected:
    bool awaiting_reply{ false };
    bool show_dialogs{ true };

    virtual void parseReply(const std::string& reply) = 0;
    std::string getTextNullCheck(const TiXmlElement* e);

public:
    void getReply(const std::string& reply);
};

