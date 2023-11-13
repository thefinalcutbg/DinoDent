#pragma once

#include "HisService.h"
#include <functional>
#include "Model/Patient.h"

namespace EHospitalization
{
	class Fetch : private HisService
	{
		std::function<void(bool)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Fetch() :
			HisService("H001", "/v1/ehospitalization/hospitalization/fetch") {}

		bool sendRequest(const Patient& patient, const std::string& rzi, const Date& from, const Date& to, decltype(m_callback) callback);
		bool awaitingReply() { return awaiting_reply; }
	};
}