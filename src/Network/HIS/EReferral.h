#pragma once
#include <functional>
#include "Model/Referrals/Referral.h"
#include "HisService.h"

namespace EReferral {


	class Issue : private HisService
	{
		std::function<void(const std::string&)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Issue() :
			HisService("R001", "/v3/ereferral/doctor/issue") {}

		bool sendRequest(
			const std::string& examNrn,
			const Patient& patient,
			const Referral& ref,
			std::function<void(const std::string&)> nrnCallback
		);

	};

	class Cancel : private HisService
	{

		std::function<void(bool)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Cancel() :
			HisService("R007", "/v3/ereferral/doctor/cancel") {}

		bool sendRequest(const std::string& nrn, std::function<void(bool)> success);
	};

}