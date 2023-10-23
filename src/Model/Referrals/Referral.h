#pragma once

#include <vector>
#include <variant>
#include <array>
#include "Model/Date.h"
#include "Model/Dental/MKB.h"
#include "Model/Dental/KSMP.h"
#include "ReferralData.h"
#include "Model/FreeFunctions.h"


class Referral
{
public:

	typedef std::variant<MDD4Data, R3Data, R3AData, MH119Data> RefData;

private:

	static RefData initData(ReferralType t) {

		switch (t)
		{
			case ReferralType::MDD4: return MDD4Data();
			case::ReferralType::Form3: return R3Data();
			case ReferralType::Form3A: return R3AData();
			case ReferralType::MH119: return MH119Data();
		}
	}

public:

	static inline constexpr std::array<const char*, 4> refDescription{
		"Направление за рентгенография (бл.4)",
		"Направление за психиатър (бл.3)",
		"Направление за анестезиолог (бл.3А)",
		"Направление за СИДП (бл.119МЗ)"
	};

	long long rowid{ 0 };

	int number{ 0 };
	std::string nrn;
	std::string lrn;


	Date date{ Date::currentDate() };

	RefDiagnosis diagnosis;
	RefDiagnosis comorbidity;

	Reason reason{ 1 };

	ReferralType type;

	RefData data;

	Referral(ReferralType t) : type(t), data(initData(t)) {};

	const char* getTypeAsString() const {

		return refDescription[static_cast<int>(type)];
	};
	

	inline bool isNrnType() const { 
		return type != ReferralType::MH119;
	}

	static bool isNrnType(ReferralType type) {
		return type != ReferralType::MH119;
	}

	inline bool isSentToHIS() const {
		return nrn.size();
	}

	std::string getNumber() const;
};
