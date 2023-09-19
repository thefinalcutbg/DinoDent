#pragma once

#pragma once

#include "HisService.h"
#include <functional>

struct MedicalNotice;
struct Patient;

namespace EMedicalNotice {

	class Issue : private HisService
	{
		std::function<void(const std::string&)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Issue() :
			HisService("C041", "/v3/commons/doctor/medical-notice-issue") {}

		bool sendRequest(
			const MedicalNotice& notice,
			const Patient& patient,
			const std::string examNrn,
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
			HisService("C045", "/v3/commons/doctor/medical-notice-cancel") {}

		bool sendRequest(
			const std::string nrn,
			std::function<void(bool)> callback
		);
	};
};
