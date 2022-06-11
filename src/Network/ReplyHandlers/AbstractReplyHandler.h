#pragma once

#include <string>
#include "../Network.h"
class TiXmlElement;

class AbstractReplyHandler
{

public:
	virtual void getReply(const std::string& reply) = 0;
	~AbstractReplyHandler() {
		Network::unsubscribeHandler(this);
	}

};

