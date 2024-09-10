#pragma once

#include "HisService.h"
#include <functional>
#include "Model/Dental/AmbList.h"
#include "Model/Dental/HisSnapshot.h"

namespace EDental {

	class Open : private HisService
	{
		std::function<void(const std::string& nrn, const std::vector<std::pair<int, int>>& seqIdxPair, bool error)> m_callback;

		std::string getProcedures(const ProcedureContainer& procedures, const ToothContainer& teeth, const Date& treatmentStartDate);

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Open() :
			HisService("D001", "/v1/edental/record/open") {}

		bool sendRequest(
			const AmbList& ambSheet,
			const Patient& patient,
			decltype(m_callback) nrnCallback
		);

	};

	class Augment : private HisService
	{
		std::function<void(const std::vector<std::pair<int, int>>& seqIdxPair)> m_callback;

		std::string getProcedures(const ProcedureContainer& procedures, const ToothContainer& teeth, const Date& treatmentStartDate, bool autoStatusRemove);

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Augment() :
			HisService("D003", "/v1/edental/record/augment") {}

		bool sendRequest(
			const AmbList& ambSheet,
			const Patient& patient,
			bool autoStatus, //legacy stuff
			decltype(m_callback) callback
		);
	};

	class Cancel : private HisService
	{

		std::function<void(bool)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Cancel() :
			HisService("D007", "/v1/edental/record/cancel") {}

		bool sendRequest(const std::string& nrn, std::function<void(bool)> success);
	};

	class GetStatus : private HisService
	{

		std::function<void(const ToothContainer& teeth)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		GetStatus() :
			HisService("D009", "/v1/edental/status/get") {}

		bool sendRequest(const Patient& patient, std::function<void(const ToothContainer&)> callback);
	};

	class GetProcedures : private HisService
	{

		std::function<void(const std::optional<std::vector<Procedure>>& procedures)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		GetProcedures() :
			HisService("D009", "/v1/edental/status/get") {}

		bool sendRequest(const Patient& patient, bool showDialog, decltype(m_callback) callback);

		bool awaitingReply() { return awaiting_reply; }
	};

	//returns procedures AND status
	class GetStatusAndProcedures : private HisService
	{

		std::function<void(const std::optional<std::vector<Procedure>>& procedures, const ToothContainer& teeth)> m_callback;

		void parseReply(const std::string& reply) override;

	public:
		GetStatusAndProcedures() :
			HisService("D009", "/v1/edental/status/get") {}

		bool sendRequest(const Patient& patient, bool showDialogs, decltype(m_callback) callback);

		bool awaitingReply() { return awaiting_reply; }
	};

	class GetDentalHistory : private HisService
	{
		std::function<void(const std::vector<HisSnapshot>& snapshots)> m_callback;

		void parseReply(const std::string& reply) override;

	public:
		GetDentalHistory() :
			HisService("D009", "/v1/edental/status/get") {}

		bool sendRequest(const Patient& patient, decltype(m_callback) callback);

		bool awaitingReply() { return awaiting_reply; }
	};

	class Fetch : private HisService
	{
		std::function<void(const AmbList& list, const Patient& patient)> m_callback;

		void parseReply(const std::string& reply) override;

	public:
		Fetch() :
			HisService("D005", "/v1/edental/record/fetch") {}

		bool sendRequest(const std::string& nrn, decltype(m_callback) callback);

		bool awaitingReply() { return awaiting_reply; }
	};
}