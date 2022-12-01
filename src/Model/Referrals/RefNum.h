#pragma once

#include "Model/NumenclatureTemplate.h"
#include "Model/Dental/MKB.h"

namespace RefNumPrv {

	extern std::array<const char*, 2> reason;
}

typedef NumStatic<2, RefNumPrv::reason> Reason;

struct RefDiagnosis {
	MKB main;
	MKB additional;
};

enum class ReferralType { MDD4, Form3, Form3A };//, Hospitalization, Expertise };