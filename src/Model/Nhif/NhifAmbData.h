#pragma once
#include <string>

enum class Charge { standard, retired, freed };

enum class Specification {
	PRIMARY_NORM, //код специалнсот 60 или 64 и цената се покрива частично/изцяло от НЗОК
	PRIMARY_SPEC, //код специалнсот 60 или 64 и цената се покрива ИЗЦЯЛО от НЗОК
	SPEC_NORM,    //код специалнсот 61, 62 или 68 и цената се покрива частично/изцяло от НЗОК
	SPEC_DOMOVE,  //код специалнсот 61, 62 или 68 и цената се покрива ИЗЦЯЛО от НЗОК
	SPEC_PS       //за обща анестезия (такъв няма feature все още)
};

class NhifAmbData
{
	Specification m_spec{ Specification::PRIMARY_NORM };
	bool full_coverage{ false };
	
public:

	NhifAmbData();
	NhifAmbData(Specification spec);

	//no need for charge and pregnancy to be private, since there is no complex logic behind the values
	Charge charge{ Charge::standard };
	bool pregnancy{ false };

	void setFullCoverage(bool full);
	void setAnesthesia(bool anesthesia);
	std::string getSpecTag() const;

	Specification spec() const { return m_spec; }
	bool getFullCoverage() const { return full_coverage; }


};