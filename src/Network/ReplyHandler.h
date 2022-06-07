#pragma once

#include <string>
#include "Network.h"

class ReplyHandler
{
public:
	virtual void getReply(const std::string& reply) = 0;
	~ReplyHandler() {
		Network::unsubscribeHandler(this);
	}

};

