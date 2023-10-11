#pragma once
#include <string>
#include "Model/Referrals/Referral.h"

class DateValidator;

class IReferralDialog 
{
public:


	struct CommonData {
		Date date;
		int number;
		std::string mkbMain;
		std::string mkbAdditional;
		std::string mkbComorbMain;
		std::string mkbComorbAdd;
		int reason_idx;
	};


	virtual void setTitle(const std::string& str) = 0;
	virtual void setReferral(const Referral& referral) = 0;
	virtual MDD4Data MDD4data() = 0;
	virtual MH119Data MH119data() = 0;
	virtual CommonData getCommon() = 0;
	virtual void closeDialog() = 0;
	virtual void setErrorLabel(const std::string& str) = 0;
	virtual void setDateValidator(DateValidator* d) = 0;
	
};