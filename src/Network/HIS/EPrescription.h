#pragma once

#include "HisService.h"
#include <functional>

struct Prescription;
struct Patient;

namespace EPrescription {

	enum Status { None, Active, PartiallyComplete, Complete, Rejected, Canceled, PartiallyCanceled, Expired, Size };

	std::string getStatusText(EPrescription::Status s);

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

	class FetchDispense : private HisService
	{
		std::function<void(EPrescription::Status s)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		FetchDispense() :
			HisService("P013", "/v3/eprescription/pharmacy/fetchdispense") {}

		bool sendRequest(const std::string& nrn, std::function<void(EPrescription::Status s)> success);
	};

	class eRxFetch : private HisService
	{
		std::function<void(const Prescription& prescr)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		eRxFetch() :
			HisService("P003", "/v3/eprescription/pharmacy/fetch") {}

		bool sendRequest(
			const std::string& nrn,
			const std::string& lpk,
			const Patient& p,
			decltype(m_callback) callback
		);
	};


}