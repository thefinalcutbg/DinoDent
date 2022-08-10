#include "NhifAmbData.h"
#include "Model/User/User.h"


NhifAmbData::NhifAmbData()
{
	if (User::doctor().isSpecialist())
		m_spec = Specification::SPEC_NORM;
}

NhifAmbData::NhifAmbData(Specification spec) :
	m_spec(spec), 
	full_coverage(spec == Specification::PRIMARY_SPEC || spec == Specification::SPEC_NORM)
{}

void NhifAmbData::setAnesthesia(bool anesthesia)
{
	if (anesthesia) m_spec = Specification::SPEC_PS;
	else setFullCoverage(full_coverage);
}

std::string NhifAmbData::getSpecTag() const
{
	static constexpr const char* specType[5]
	{
		"PRIMARY NORM",
		"PRIMARY SPEC",
		"SPEC NORM",
		"SPEC DOMOVE",
		"SPEC_PS"  
	};

	return specType[static_cast<int>(m_spec)];
}

void NhifAmbData::setFullCoverage(bool full)
{
	if (full) {

		User::doctor().isSpecialist() ?
			m_spec = Specification::SPEC_DOMOVE
			:
			m_spec = Specification::PRIMARY_SPEC;
	}
	else
	{
		User::doctor().isSpecialist() ?
			m_spec = Specification::SPEC_NORM
			:
			m_spec = Specification::PRIMARY_SPEC;
	}
	
}

