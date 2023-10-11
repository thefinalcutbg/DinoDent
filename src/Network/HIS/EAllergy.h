#pragma once

#include "HisService.h"
#include <functional>
#include "Model/Allergy.h"

namespace EAllergy
{
	class Report : private HisService
	{
		std::function<void(const std::string& nrn)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Report() :
			HisService("C023", "/v3/commons/doctor/allegies-report") {}

		bool sendRequest(const Patient& patient, const std::string& rzi, const Allergy& allergy, decltype(m_callback) callback);
		bool awaitingReply() { return awaiting_reply; }
	};

	class Edit : private HisService
	{
		std::function<void(bool success)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Edit() :
			HisService("C025", "/v3/commons/doctor/allegies-edit") {}

		bool sendRequest(const Allergy& allergy, decltype(m_callback) callback, bool enteredInError = false);
		bool awaitingReply() { return awaiting_reply; }
	};

	class Fetch : private HisService
	{
		std::function<void(const std::vector<Allergy>& allergies)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Fetch() :
			HisService("C027", "/v3/commons/doctor/allegies-fetch") {}

		bool sendRequest(const Patient& patient, const std::string& rzi, decltype(m_callback) callback);
		bool awaitingReply() { return awaiting_reply; }
		
	};



}