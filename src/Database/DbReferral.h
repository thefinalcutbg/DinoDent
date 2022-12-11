#pragma once
#include "Model/Referrals/Referral.h"

class Db;

namespace DbReferral 
{
	int getRefNumber(ReferralType type, int year);
	bool refNumberExists(ReferralType type, int year, long long refRowId);
	std::vector<Referral> getReferrals(long long ambListId, Db& db);
	void saveReferrals(const std::vector<Referral>& ref, long long ambListId, Db& db);

}