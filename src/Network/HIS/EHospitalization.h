#pragma once

#include "HisService.h"
#include <functional>
#include "Model/Patient.h"
#include "Model/Hospitalization.h"

namespace EHospitalization
{
	class Fetch : private HisService
	{
		std::function<void(std::vector<Hospitalization>)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Fetch() :
			HisService("H001", "/v1/ehospitalization/hospitalization/fetch") {}
		
		bool sendRequest(const Patient& patient, const std::string& rzi, decltype(m_callback) callback);
		bool awaitingReply() { return awaiting_reply; }
		using HisService::show_dialogs;
	};
}