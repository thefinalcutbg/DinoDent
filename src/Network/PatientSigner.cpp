#include "PatientSigner.h"

#include <QtVersion>

#ifdef Q_OS_WIN
#include "crypto.h"
#include <QAxWidget>
#include <QAxObject>
#include "Model/Patient.h"
#include "View/ModalDialogBuilder.h"
#include <QPixmap>
#include <QMessageBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QCryptographicHash>
#include <QApplication>
#include <windows.h>
#include <chrono>
#include <thread>
#include "signoTec/STPadLib.h"

PatientSignature PatientSigner::signWithWacom(const std::string& what, const std::string& who, const std::string& why) {

    const QString licence = "eyJhbGciOiJSUzUxMiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI3YmM5Y2IxYWIxMGE0NmUxODI2N2E5MTJkYTA2ZTI3NiIsImV4cCI6MjE0NzQ4MzY0NywiaWF0IjoxNTYwOTUwMjcyLCJyaWdodHMiOlsiU0lHX1NES19DT1JFIiwiU0lHQ0FQVFhfQUNDRVNTIl0sImRldmljZXMiOlsiV0FDT01fQU5ZIl0sInR5cGUiOiJwcm9kIiwibGljX25hbWUiOiJTaWduYXR1cmUgU0RLIiwid2Fjb21faWQiOiI3YmM5Y2IxYWIxMGE0NmUxODI2N2E5MTJkYTA2ZTI3NiIsImxpY191aWQiOiJiODUyM2ViYi0xOGI3LTQ3OGEtYTlkZS04NDlmZTIyNmIwMDIiLCJhcHBzX3dpbmRvd3MiOltdLCJhcHBzX2lvcyI6W10sImFwcHNfYW5kcm9pZCI6W10sIm1hY2hpbmVfaWRzIjpbXX0.ONy3iYQ7lC6rQhou7rz4iJT_OJ20087gWz7GtCgYX3uNtKjmnEaNuP3QkjgxOK_vgOrTdwzD-nm-ysiTDs2GcPlOdUPErSp_bcX8kFBZVmGLyJtmeInAW6HuSp2-57ngoGFivTH_l1kkQ1KMvzDKHJbRglsPpd4nVHhx9WkvqczXyogldygvl0LRidyPOsS5H2GYmaPiyIp9In6meqeNQ1n9zkxSHo7B11mp_WXJXl0k1pek7py8XYCedCNW5qnLi4UCNlfTd6Mk9qz31arsiWsesPeR9PN121LBJtiPi023yQU8mgb9piw_a-ccciviJuNsEuRDN3sGnqONG3dMSA";

    QAxWidget ctl("Florentis.SigCtl");

    ctl.setProperty("Licence", licence);

    QAxObject dyn("Florentis.DynamicCapture");
    dyn.setProperty("Licence", licence);

    //CANONICALIZATION - NOT WORKING?
    /*
    std::string canonicalized = Crypto::addNamespacesToRoot(what, { {"nhis", "https://www.his.bg" } });

    canonicalized = Crypto::canonicalizeXML(canonicalized);

    ModalDialogBuilder::showMultilineDialog(canonicalized);
    */
    //SETTING HASH

    QAxObject hash("Florentis.Hash");
    hash.setProperty("Type", 4);
    hash.dynamicCall("Add(Data)", QString(what.data()));

    //CAPTURING SIGNATURE

    QVariant resultVariant = dyn.dynamicCall("Capture(SigCtl, Who, Why, What, Key)",
        ctl.asVariant(),
        who.data(), why.data(),
        hash.asVariant(), 0
    );

    int result = resultVariant.toInt();

    {
        if (result != 0) return {};
    }

    //GETTING BITMAP

    auto sigObject = ctl.querySubObject("Signature");

    QVariantList args;
    args << ""
        << 300 << 300
        << QString("image/png")
        << 0.5
        << QColor(Qt::black)
        << QColor(Qt::white)
        << 0.0 << 0.0
        << (0x000800 | 0x080000 | 0x010000 | 0x100000);
    
    auto bitmap = sigObject->dynamicCall(
        "RenderBitmap(outputFilename, dimensionX, dimensionY, mimeType, inkWidth, nkColor, backgroundColor, paddingX, paddingY, flags) ", args).toByteArray();

	//GETTING SIGNATURE

    return PatientSignature{
        .signatureObject = sigObject->property("SigText").toString().toStdString(),
        .sigImage = std::vector<unsigned char>(
            reinterpret_cast<const unsigned char*>(bitmap.constData()),
            reinterpret_cast<const unsigned char*>(bitmap.constData()) + bitmap.size())
    };

}

PatientSignature PatientSigner::signWithSignotec(const std::string& what)
{
    PatientSignature ps; 

    long size = 0; //stores the size of the buffer

    std::vector<unsigned char> buffer(size);


    if (STDeviceOpen(0, true) < 0) {
        return ps;
    }

    STSensorSetSignRect(0, 0, 0, 0);

    //SETTING HASH

//    auto canonicalized = Crypto::addNamespacesToRoot(what, { {"nhis", "https://www.his.bg" } });

//    canonicalized = Crypto::canonicalizeXML(canonicalized);

    auto hashedDocument = Crypto::calculateSHA256Digest(what);

    STRSASetHash(reinterpret_cast<unsigned char*>(what.data()), HASHALGO::kSha256, 0);

    //CAPTURING SIGNATURE:

    if (STSignatureStart() < 0) {
        STDeviceClose(0);
        STControlExit();
        return ps;
    }

	int dialogResult = 1; // 0 - OK, 1 - Retry, -1 or 2 - Cancel
    
    while (dialogResult == 1) 
    {
        dialogResult = ModalDialogBuilder::openButtonDialog(
            { "OK", "Повтори", "Отказ" }, "Подпис на пациент"
        );

        if (dialogResult == 0) break;

        if (dialogResult == 2 || dialogResult == -1) {
            STSignatureStop();
            STDeviceClose(0);
            STControlExit();
            return ps;
        }

        //dialog result 1
        STSignatureRetry();
    }

    STSignatureConfirm();

    //GETTING SIGNATURE

    STRSASign(nullptr, &size, RSASCHEME::kPSS, HASHVALUE::kCombination, 0);

    buffer = std::vector<unsigned char>(size);

    STRSASign(buffer.data(), &size, RSASCHEME::kPSS, HASHVALUE::kCombination, 0);

    ps.signature = Crypto::base64Encode(buffer);

    //GETTING SIGN DATA

    STSignatureGetSignData(nullptr, &size);

    if (!size)
    {
        STDeviceClose(0);
        STControlExit();
        return ps;
    }

    buffer = std::vector<unsigned char>(size);

    STSignatureGetSignData(buffer.data(), &size);

    ps.signatureObject = Crypto::base64Encode(buffer);

    //GETTING THE CERTIFICATE

    STRSASaveSigningCertAsStream(nullptr, &size, CERTTYPE::kCert_DER);

    buffer = std::vector<unsigned char>(size);

    STRSASaveSigningCertAsStream(buffer.data(), &size, CERTTYPE::kCert_DER);

    ps.signatureCertificate = Crypto::base64Encode(buffer);

    //GETTING BITMAP
                   
    STSignatureSaveAsStreamEx(nullptr, &size, 300, 0, 0, kPng, 0, RGB(0, 0, 255), 0);

    ps.sigImage.resize(size);

    STSignatureSaveAsStreamEx(ps.sigImage.data(), &size, 300, 0, 0, kPng, 0, RGB(0, 0, 255), 0);

    STDeviceClose(0);
    STControlExit();

    return ps;
}

#else
PatientSignature PatientSigner::signWithSignotec(const std::string& what)
{
	return {};
}

PatientSignature PatientSigner::signWithWacom(const std::string& what, const std::string& who, const std::string& why) {
	return {};
}

#endif