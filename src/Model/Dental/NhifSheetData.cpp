﻿#include "NhifSheetData.h"
#include "Model/UserStructs.h"

std::string NhifSheetData::getSpecString(NhifSpecialty specialty) const
{
    if (specialty == NhifSpecialty::None) throw std::exception("WRONG SPECIALTY");

    //код специалнсот 64 и цената се покрива частично/изцяло от НЗОК:
    if (specialty == NhifSpecialty::General && specification == NhifSpecification::PartialCoverage) return "PRIMARY NORM";
    //код специалнсот 64 и цената се покрива ИЗЦЯЛО от НЗОК:
    if (specialty == NhifSpecialty::General && specification == NhifSpecification::FullCoverage) return "PRIMARY SPEC";
    //код специалност 64 обща анестезия:
    if (specialty == NhifSpecialty::General && specification == NhifSpecification::Anesthesia) return "PRIMARY PS";
    //код специалнсот 61, 62 или 68 и цената се покрива частично/изцяло от НЗОК:
    if (specification == NhifSpecification::PartialCoverage) return "SPEC NORM";
    //код специалнсот 61, 62 или 68 и цената се покрива ИЗЦЯЛО от НЗОК:
    if (specification == NhifSpecification::FullCoverage) return "SPEC DOMOVE";
    //од специалнсот 61, 62 или 68 обща анестезия
    if (specification == NhifSpecification::Anesthesia) return "SPEC PS";

}