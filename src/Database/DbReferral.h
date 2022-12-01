#pragma once
#include "Model/Referrals/Referral.h"

namespace DbReferral 
{
	int getRefNumber(ReferralType type, int year);
	bool refNumberExists(ReferralType type, int year, long long refRowId);

}