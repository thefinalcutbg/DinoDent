#pragma once

#include "HisService.h"
#include <functional>

struct Prescription;
struct Patient;

namespace EPrescription {

	class Issue : private HisService
	{
		std::function<void(const std::string&)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Issue() :
			HisService("P001", "/v3/eprescription/doctor/issue") {}

		bool sendRequest(
			const Prescription& perscr, 
			const Patient& patient,
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
			HisService("P007", "/v3/eprescription/doctor/cancel") {}

		bool sendRequest(const std::string& nrn, std::function<void(bool)> success);
	};

}