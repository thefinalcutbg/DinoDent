#include "SignatureTablet.h"

SignatureTablet::SignatureTablet(int idx)
{
	if (idx >= s_models.size() || idx < 0) return;

	m_modelIdx = idx;
}

SignatureTablet::Manufacturer SignatureTablet::getHisManifacturer() const
{
	if (!m_modelIdx) return NONE;
	if (m_modelIdx < 5) return EVOLIS;
	if (m_modelIdx < 11) return SIGNOTEC;

	return WACOM;
}

std::string SignatureTablet::defaultPDFSignerLocation() const
{
	auto manif = getHisManifacturer();

	if (manif == WACOM) return "C:/Program Files/Wacom sign pro PDF/Sign Pro PDF.exe";

	if (manif == NONE) return std::string();

	return "C:/Program Files/signotec/signoSign2/signoSign2.exe";
}
