#include "DentalNum.h"

std::array<const char*, 10> PrvNum::restoration_material = {
		"",
		"Фотополимер",
		"Химиополимер",
		"Двойнополимеризиращ композит",
		"Глас-Йономерен Цимент",
		"Bulk-Fill",
		"Компомер",
		"Амалгама",
		"Керамика",
		"Метал"

};

std::array<const char*, 9> PrvNum::crown_material = {
	"",
	"Металокерамика",
	"Керамична инкрустация",
	"Пластмасова инкрустация",
	"Прескерамика",
	"Цирконий",
	"Пластмаса",
	"Метал",
	"Злато"
};

std::array<const char*, 5> PrvNum::crown_prep = {
	"",
	"Безпрагово изпиляване",
	"Прагово изпиляване",
	"Завинтване върху имплант",
	"Циментиране върху имплант"
};

std::array<const char*, 4> PrvNum::post_type = {
	"",
	"Пасивен щифт",
	"Активен щифт",
	"Фиброщифт"
};

std::array<const char*, 4> PrvNum::implant_type = {
	"",
	"Закрито",
	"Цикатризационен винт",
	"Временна корона"
};

std::array<const char*, 4> PrvNum::implant_time = {
	"",
	"Отложено",
	"Ранно",
	"Имедиантно"
};

std::array<const char*, 4> PrvNum::tissue_aug = {
	"",
	"Roll Flap",
	"Free Gingival Graft",
	"Connective Tissue Graft"
};

std::array<const char*, 5> PrvNum::bone_aug = {
	"",
	"Ксенографт",
	"Алографт",
	"Автографт",
	"Бета - 3 - калциев фосфат"
};

std::array<const char*, 9> PrvNum::caries_diag = {
	"Caries media",
	"Caries secunda",
	"Caries superficialis",
	"Caries profunda",
	"Caries profundissima",
	"Caries cervicalis",
	"Caries recidiva",
	"Macula cariosa alba",
	"Macula cariosa fusca"
};

std::array<const char*, 19> PrvNum::pulpitis_diag = {
	"Pulpitis chronica ulcerosa",
	"Necrosis pulpae",
	"Pulpitis acuta serosa partialis",
	"Pulpitis acuta serosa totalis",
	"Pulpitis acuta purulenta partialis",
	"Pulpitis acuta purulenta totalis",
	"Pulpitis acuta gangraenosa",
	"Pulpitis chronica granulomatosa",
	"Pulpitis chronica exacerbata",
	"Pulpoperiodontitis",
	"Pulpitis traumatica",
	"Colisio pulpae",
	"Haemorrhagia pulpae",
	"Hyperaemia pulpae",
	"Atrophia pulpae",
	"Gangraena simplex",
	"Gangraena complicata",
	"Gangraena aperta ",
	"Gangraena clausa"
};

std::array<const char*, 9> PrvNum::lesion_diag = {
	"Periodontitis chronica granulomatosa localisata",
	"Periodontitis chronica granulomatosa difusa",
	"Periodontitis chronica exacerbata",
	"Periodontitis chronica fibrosa",
	"Periodontitis periapicalis acuta serosa",
	"Periodontitis periapicalis acuta  purulenta",
	"Periodontitis marginalis acuta serosa ",
	"Periodontitis marginalis acuta purulenta",
	"Periodontitis totalis acuta"
};

std::array<const char*, 5>PrvNum::fracture_diag = {
	"Fractura coronae dentis",
	"Fractura coronae dentis cum collisione pulpae",
	"Fractura radicis dentis",
	"Fractura radicis dentis verticalis",
	"Cracked tooth syndrome"
};

std::array<const char*, 2>PrvNum::radix_diag = {
	"Radix dentis",
	"Radix relicva"
};