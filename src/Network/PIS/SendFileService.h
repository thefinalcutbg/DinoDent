#pragma once
#include "PisService.h"

struct Invoice;

class SendFileService final : private PisService
{
	void parseReply(const std::string& reply) override;

public:
	void sendInvoice(const Invoice& invoice, const std::string& rziCode);
	void sendAmbReport(const std::string& data, const std::string& doctorLPK);
};

