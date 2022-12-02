#pragma once

#include <vector>
#include <variant>
#include <array>
#include "Model/Date.h"
#include "Model/Dental/MKB.h"
#include "Model/Dental/KSMP.h"
#include "ReferralData.h"



class Referral
{
public:

	typedef std::variant<MDD4Data, R3Data, R3AData> RefData;

private:

	static RefData initData(ReferralType t) {

		switch (t)
		{
			case ReferralType::MDD4: return MDD4Data();
			case::ReferralType::Form3: return R3Data();
			case ReferralType::Form3A: return R3AData();
		}
	}

public:

	long long rowid{ 0 };

	int number{ 0 };

	Date date{ Date::currentDate() };

	RefDiagnosis diagnosis;

	Reason reason{ 0 };

	ReferralType type;

	RefData data;

	Referral(ReferralType t) : type(t), data(initData(t)) {};

	const char* getTypeAsString() const {

		static constexpr const char* refNames[3]{
			"Направление за МДД (бл.4)",
			"Направление за психиатър (бл.3)",
			"Направление за анестезиолог (бл.3А)"
		};

		return refNames[static_cast<int>(type)];
	};
	
};
