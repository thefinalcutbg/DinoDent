#pragma once
#include "AbstractReplyHandler.h"

class MonthNotifLoader;

class MonthNotifDataHandler : public AbstractReplyHandler
{
	MonthNotifLoader* loader;

public:
	MonthNotifDataHandler(MonthNotifLoader* l);

	void getReply(const std::string& reply) override;

};

