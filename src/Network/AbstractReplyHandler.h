#pragma once

#include <string>

class TiXmlElement;

class AbstractReplyHandler
{

protected:
	bool awaiting_reply{ false };

	virtual void parseReply(const std::string& reply) = 0;
	std::string getTextNullCheck(const TiXmlElement* e);

public:
	void getReply(const std::string& reply);

	~AbstractReplyHandler();
};
