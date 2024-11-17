#include "GetHSM.h"
#include "View/ModalDialogBuilder.h"
#include "Network/NetworkManager.h"
#include "View/Widgets/CertificateListDialog.h"

bool s_multi_pkcs11 = false;

std::optional<PKCS11> GetHSM::get(bool showDialogs)
{

	auto pinLambda = [](const X509Details& cert) {

		return ModalDialogBuilder::pinPromptDialog(cert.x509_pem, cert.driverPath);
	};

	auto certLambda = [](const std::vector<X509Details>& certList) {

		CertificateListDialog d(certList);
		d.exec();

		return d.getIndex();
	};

	PKCS11 hsm(pinLambda, s_multi_pkcs11 ? certLambda : nullptr);

	switch(hsm.getState()) {

		case PKCS11::NoCertificate:
			if (showDialogs) {
				ModalDialogBuilder::showMessage("Не е открит КЕП");
			}
			return {};
		case PKCS11::NoCertificateChosen:
			return {};
		case PKCS11::LoginAborted:
			return {};
		case PKCS11::LoginFailed:
			ModalDialogBuilder::showError("Грешна парола или блокирана карта");
			return {};
		case PKCS11::JustLoggedIn:

			NetworkManager::clearAccessCache();
			return hsm;

		case PKCS11::LoggedIn:
			return hsm;
	}

	return {};

}

void GetHSM::setMultiPKCS11(bool multi)
{
	s_multi_pkcs11 = multi;
}
