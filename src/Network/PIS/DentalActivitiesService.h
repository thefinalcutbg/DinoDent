#pragma once
#include "PisService.h"

#include <functional>
#include <optional>
#include "Model/Dental/Procedure.h"

class DentalActivitiesService final: private PisService
{
	
	std::function<
		void(
			const std::optional<std::vector<Procedure>>&, 
			const std::vector<std::string>& payment_status
			)
	> m_callback;

	void parseReply(const std::string& reply) override;
public:

	bool sendRequest(
		int personType,
		const std::string& patientId,
		std::function<void(const std::optional<std::vector<Procedure>>&, const std::vector<std::string>& payment_status)> callback
	);

};
