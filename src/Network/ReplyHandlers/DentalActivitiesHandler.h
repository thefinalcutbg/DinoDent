#pragma once
#include "AbstractReplyHandler.h"
#include "Model/Date.h"
#include "Model/Procedure/Procedure.h"

struct SimpleProcedure {

	Date date;
	std::string code;
	int tooth;
	int idx; //the index in the array system (0-31 or 99 for general)
	bool temp;
	std::string diagnosis;
};


class ListPresenter;

class DentalActivitiesHandler : public AbstractReplyHandler
{
	ListPresenter* p_presenter;

public:
	DentalActivitiesHandler(ListPresenter* p);

	void getReply(const std::string& reply) override;

};

