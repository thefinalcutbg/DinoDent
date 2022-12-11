#pragma once

#include "Model/NumenclatureTemplate.h"
#include "Model/Dental/MKB.h"

namespace RefNumPrv {

	extern std::array<const char*, 2> reason;
	extern std::array<const char*, 4> MH119_reason;
}

typedef NumStatic<2, RefNumPrv::reason> Reason;
typedef NumStatic<4, RefNumPrv::MH119_reason> MH119Reason;

struct RefDiagnosis {
	MKB main;
	MKB additional;

	std::string getText() const {
		return main.name() + " " + additional.name();
	}

};

enum class ReferralType { MDD4, Form3, Form3A, MH119 };//, Hospitalization, Expertise };