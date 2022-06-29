#pragma once

#include <string>
#include "../Network.h"
class TiXmlElement;

class AbstractReplyHandler
{
protected:

	std::string getTextNullCheck(const TiXmlElement* e);

public:
	virtual void getReply(const std::string& reply) = 0;

	//intended to be set only from the network;
	bool awaiting_reply{ false };


	~AbstractReplyHandler() {
		Network::unsubscribeHandler(this);
	}

};

