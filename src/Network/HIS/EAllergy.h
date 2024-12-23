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
            HisService("L023", "/v1/longterm-care/doctor/allegies-report") {}

        bool sendRequest(const Patient& patient, const Allergy& allergy, decltype(m_callback) callback);
		bool awaitingReply() { return awaiting_reply; }
	};

	class Edit : private HisService
	{
		std::function<void(const std::string& nrn)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Edit() :
            HisService("L025", "/v1/longterm-care/doctor/allegies-edit") {}

		bool sendRequest(const Allergy& allergy, decltype(m_callback) callback);
		bool awaitingReply() { return awaiting_reply; }
	};

	class Fetch : private HisService
	{
		std::function<void(const std::vector<Allergy>& allergies)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Fetch() :
            HisService("L027", "/v1/longterm-care/doctor/allegies-fetch") {}
		//does not return "Entered in Error"
		bool sendRequest(const Patient& patient, const std::string& rzi, decltype(m_callback) callback);
		bool awaitingReply() { return awaiting_reply; }
		
	};



}
