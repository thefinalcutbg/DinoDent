#pragma once
#include "PisService.h"

#include <functional>
#include <optional>
#include "Model/Dental/Procedure.h"

class DentalActivitiesService final: public PisService
{
	
	std::function<void(const std::optional<std::vector<Procedure>>&)> m_callback;

	void processPISReply(const std::string& reply) override;
public:

	//returns procedures with payment status as a note
	bool sendRequest(
		int personType,
		const std::string& patientId,
		bool showDialogs,
		decltype(m_callback) callback
	);


};
