#pragma once
#include "Model/NumenclatureTemplate.h"

namespace PrvNum {

	extern std::array<const char*, 10> restoration_material;
	extern std::array<const char*, 9> crown_material;
	extern std::array<const char*, 5> crown_prep;
	extern std::array<const char*, 4> post_type;
	extern std::array<const char*, 4> implant_type;
	extern std::array<const char*, 4> implant_time;
	extern std::array<const char*, 4> tissue_aug;
	extern std::array<const char*, 5> bone_aug;
	extern std::array<const char*, 9> caries_diag;
	extern std::array<const char*, 19> pulpitis_diag;
	extern std::array<const char*, 9> lesion_diag;
	extern std::array<const char*, 5> fracture_diag;
	extern std::array<const char*, 2> radix_diag;
} 
/*
//works, but intellisense can't parse it
typedef Numenclature<
	{""},
	{"Фотополимер"},
	{"Химиополимер"},
	{"Двойнополимеризиращ композит"},
	{"Глас-Йономерен Цимент"},
	{"Компомер"},
	{"Амалгама"},
	{"Керамика"},
	{"Метал"}
> RestorationMaterial;
*/
typedef NumStatic
<PrvNum::restoration_material.size(), PrvNum::restoration_material> RestorationMaterial;

typedef NumStatic
<PrvNum::crown_material.size(), PrvNum::crown_material> CrownMaterial;

typedef NumStatic
<PrvNum::crown_prep.size(), PrvNum::crown_prep> CrownPreparation;

typedef NumStatic
<PrvNum::post_type.size(), PrvNum::post_type> PostType;

typedef NumStatic
<PrvNum::implant_type.size(), PrvNum::implant_type> ImplantType;

typedef NumStatic
<PrvNum::implant_time.size(), PrvNum::implant_time> ImplantTime;

typedef NumStatic
<PrvNum::tissue_aug.size(), PrvNum::tissue_aug> TissueAugmentation;

typedef NumStatic
<PrvNum::bone_aug.size(), PrvNum::bone_aug> BoneAugmentation;

typedef NumStatic
<PrvNum::caries_diag.size(), PrvNum::caries_diag> CariesDiagnosis;

typedef NumStatic
<PrvNum::pulpitis_diag.size(), PrvNum::pulpitis_diag> PulpitisDiagnosis;

typedef NumStatic
<PrvNum::lesion_diag.size(), PrvNum::lesion_diag> ApicalLesionDiagnosis;

typedef NumStatic
<PrvNum::fracture_diag.size(), PrvNum::fracture_diag> FractureDiagnosis;

typedef NumStatic
<PrvNum::radix_diag.size(), PrvNum::radix_diag> RootDiagnosis;