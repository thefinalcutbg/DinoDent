#pragma once

#include "HisService.h"
#include <functional>
#include "Model/Dental/AmbList.h"

namespace EDental {

	class Open : private HisService
	{
		std::function<void(const std::string&)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Open() :
			HisService("D001", "/v1/edental/record/open") {}

		bool sendRequest(
			const AmbList& ambSheet,
			const Patient& patient,
			std::function<void(const std::string&)> nrnCallback
		);

	};

	class Augment : private HisService
	{
		std::function<void(bool)> m_callback;

	protected:
		void parseReply(const std::string& reply) override;

	public:
		Augment() :
			HisService("D003", "/v1/edental/record/augment") {}

		bool sendRequest(
			const AmbList& ambSheet,
			const Patient& patient,
			std::function<void(bool)> success
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

}