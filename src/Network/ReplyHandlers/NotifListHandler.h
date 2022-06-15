#pragma once
#include "AbstractReplyHandler.h"

class MonthNotifLoader;

class NotifListHandler : public AbstractReplyHandler
{

	MonthNotifLoader* loader;

public:
	NotifListHandler(MonthNotifLoader* loader);
	void getReply(const std::string& reply) override;
};

