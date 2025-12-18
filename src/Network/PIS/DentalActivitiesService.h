#pragma once
#include "PisService.h"

#include <functional>
#include <optional>
#include "Model/Dental/Procedure.h"

class DentalActivitiesService final: protected PisService
{
	
	std::function<void(const std::optional<std::vector<Procedure>>&)> m_callback;

	void processPISReply(const std::string& reply) override;
public:

	//returns procedures with payment status as a note
	bool sendRequest(
		const Patient& p,
		bool showDialogs,
		decltype(m_callback) callback
	);

	using AbstractReplyHandler::show_dialogs;
};
