#include "SignatureTablet.h"

#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>
#include <QProcess>
#include <QtVersion>

#include "Model/User.h"
#include "Model/Dental/AmbList.h"
#include "Model/Prescription/Prescription.h"
#include "Model/Financial/Invoice.h"
#include "View/ModalDialogBuilder.h"

SignatureTablet::SignatureTablet(int modelIdx, const std::string& signer_filepath) :
	signer_filepath{ signer_filepath }
{
	if (modelIdx >= s_models.size() || modelIdx < 0) return;

	m_modelIdx = modelIdx;
}

bool SignatureTablet::isPDFconfigured() const
{
	if(!m_modelIdx) return false;

	QFileInfo fileInfo;
	
	return fileInfo.exists(signer_filepath.c_str());
}

SignatureTablet::Manufacturer SignatureTablet::getHisManifacturer() const
{
	if (!m_modelIdx) return NONE;
	if (m_modelIdx < 5) return EVOLIS;
	if (m_modelIdx < 11) return SIGNOTEC;

	return WACOM;
}

bool SignatureTablet::signPdf(const std::string& filepath) const
{
	if (!isPDFconfigured()) return false;

	if (!QFileInfo::exists(signer_filepath.c_str())) {
		ModalDialogBuilder::showError(
			"Софтуерът за подписване на PDF файлове не бе намерен"
		);
		return false; 
	}

	QProcess p;

	return p.startDetached(signer_filepath.c_str(), QStringList{ filepath.c_str() });
}

std::string SignatureTablet::defaultPDFSignerLocation(int modelIdx)
{

#ifndef Q_OS_WIN:
	return std::string();
#endif
	//WACOM
	if (modelIdx >  10) return "C:/Program Files/Wacom sign pro PDF/Sign Pro PDF.exe";

	//NONE
	if (modelIdx == 0) return std::string();

	//SIGNOTEC/EVOLIS:
	return "C:/Program Files/signotec/signoSign2/signoSign2.exe";
}


