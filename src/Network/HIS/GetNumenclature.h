#pragma once
#include "HisService.h"
#include <functional>

class GetNumenclature : private HisService
{
	std::function<void(bool)> m_callable;
	void parseReply(const std::string& reply) override;

protected:
	virtual bool parseNumenclature(const std::string& reply) = 0;
	GetNumenclature();
	bool sendRequest(int numenclature);

public:

	void setCallback(std::function<void(bool)> callback) {
		m_callable = callback;
	}

};
