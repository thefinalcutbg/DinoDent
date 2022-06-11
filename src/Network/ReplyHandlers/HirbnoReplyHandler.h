#pragma once
#include "AbstractReplyHandler.h"

class PatientDialogPresenter;

class HirbnoReplyHandler : public AbstractReplyHandler
{
	PatientDialogPresenter* p_presenter;
public:
	HirbnoReplyHandler(PatientDialogPresenter* presenter);

	// Inherited via ReplyHandler
	virtual void getReply(const std::string& reply) override;

};

