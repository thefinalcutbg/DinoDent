#pragma once

#include <optional>

#include "Model/Referrals/Referral.h"
#include "View/Interfaces/IReferralDialog.h"
#include "Model/Validators/DateValidator.h"

struct AmbList;

class ReferralPresenter
{
	IReferralDialog* view{ nullptr };

	std::optional<Referral> m_result;

	//data not stored in view:
	long long ref_rowid{ 0 };
	std::string lrn;
	ReferralType m_type;
	bool sentToHis{ false };

	Date ambSheetDate;
	
	DateValidator date_validator;

public:

	ReferralPresenter(const AmbList& sheet, ReferralType t);

	ReferralPresenter(const AmbList& sheet, const Referral& r);

	void okPressed();

	void setView(IReferralDialog* view);

	std::optional<Referral> openDialog();

};