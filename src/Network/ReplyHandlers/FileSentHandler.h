#pragma once
#include "AbstractReplyHandler.h"
class FileSentHandler : public AbstractReplyHandler
{
	public:
	void getReply(const std::string& reply) override;
};

