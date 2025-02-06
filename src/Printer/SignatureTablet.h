#pragma once
#include <array>
#include <string>

struct AmbList;
struct Prescription;
struct Patient;
struct Invoice;
class Date;

class SignatureTablet
{
	int m_modelIdx = 0;
	std::string signer_filepath;

public:

	SignatureTablet() {};
	SignatureTablet(int modelIdx, const std::string& signer_filepath);

	bool isPDFconfigured() const;

	enum DeclarationType { Denture, HIRBNo, Consent, GDPR };

	static std::string defaultPDFSignerLocation(int modelIdx);

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


	Manufacturer getHisManifacturer() const;
	int getHisIdx() const { return m_modelIdx; }

	bool signPdf(const std::string& filepath) const;

};
