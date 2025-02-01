#pragma once
#include <array>
#include <string>

class SignatureTablet
{
	int m_modelIdx = 0;

public:

	static inline const std::array<std::string, 14> s_models = {
		"",
		"Evolis Sig100 Lite",
		"Evolis Sig100",
		"Evolis Sig200",
		"Evolis Sig Active",
		"Signotec Sigma LITE",
		"Signotec Sigma",
		"Signotec Omega",
		"Signotec Zeta",
		"Signotec Gamma",
		"Signotec Delta",
		"Wacom STU-43x",
		"Wacom STU-53x",
		"Wacom STU-54x"
	};

	enum Manufacturer { NONE, EVOLIS, SIGNOTEC, WACOM };

	SignatureTablet(int idx);

	Manufacturer getHisManifacturer() const;
	int getHisIdx() const { return m_modelIdx; }

	std::string defaultPDFSignerLocation() const;

};