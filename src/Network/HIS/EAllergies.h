#pragma once

#include "HisService.h"
#include <functional>
#include "Model/MedicalStatuses.h"

namespace EAllergies
{
	class Fetch : private HisService
	{
		std::function<void(const std::vector<MedicalStatus>& s)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Fetch() :
			HisService("C027", "/v3/commons/doctor/allegies-fetch") {}

		bool sendRequest(const Patient& patient, const std::string& rzi, decltype(m_callback) callback);
	
	};



}