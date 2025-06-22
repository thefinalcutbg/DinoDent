#include "PatientSigner.h"
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
#include <QtVersion>


PatientSignature PatientSigner::signWithWacom(const std::string& what, const std::string& who, const std::string& why) {

    const QString licence = "eyJhbGciOiJSUzUxMiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI3YmM5Y2IxYWIxMGE0NmUxODI2N2E5MTJkYTA2ZTI3NiIsImV4cCI6MjE0NzQ4MzY0NywiaWF0IjoxNTYwOTUwMjcyLCJyaWdodHMiOlsiU0lHX1NES19DT1JFIiwiU0lHQ0FQVFhfQUNDRVNTIl0sImRldmljZXMiOlsiV0FDT01fQU5ZIl0sInR5cGUiOiJwcm9kIiwibGljX25hbWUiOiJTaWduYXR1cmUgU0RLIiwid2Fjb21faWQiOiI3YmM5Y2IxYWIxMGE0NmUxODI2N2E5MTJkYTA2ZTI3NiIsImxpY191aWQiOiJiODUyM2ViYi0xOGI3LTQ3OGEtYTlkZS04NDlmZTIyNmIwMDIiLCJhcHBzX3dpbmRvd3MiOltdLCJhcHBzX2lvcyI6W10sImFwcHNfYW5kcm9pZCI6W10sIm1hY2hpbmVfaWRzIjpbXX0.ONy3iYQ7lC6rQhou7rz4iJT_OJ20087gWz7GtCgYX3uNtKjmnEaNuP3QkjgxOK_vgOrTdwzD-nm-ysiTDs2GcPlOdUPErSp_bcX8kFBZVmGLyJtmeInAW6HuSp2-57ngoGFivTH_l1kkQ1KMvzDKHJbRglsPpd4nVHhx9WkvqczXyogldygvl0LRidyPOsS5H2GYmaPiyIp9In6meqeNQ1n9zkxSHo7B11mp_WXJXl0k1pek7py8XYCedCNW5qnLi4UCNlfTd6Mk9qz31arsiWsesPeR9PN121LBJtiPi023yQU8mgb9piw_a-ccciviJuNsEuRDN3sGnqONG3dMSA";

    QAxWidget ctl("Florentis.SigCtl");

    ctl.setProperty("Licence", licence);

    QAxObject dyn("Florentis.DynamicCapture");
    dyn.setProperty("Licence", licence);

    QAxObject hash("Florentis.Hash");
    hash.setProperty("Type", 4);
    hash.dynamicCall("Add(Data)", what.data());

    QVariant resultVariant = dyn.dynamicCall("Capture(SigCtl, Who, Why, What, Key)",
        ctl.asVariant(),
        who.data(), why.data(),
        hash.asVariant(), 0
    );

    int result = resultVariant.toInt();

    {
        if (result != 0) return {};
    }

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

    return PatientSignature{
        .signatureObject = sigObject->property("SigText").toString().toStdString(),
        .sigImage = std::vector<unsigned char>(
            reinterpret_cast<const unsigned char*>(bitmap.constData()),
            reinterpret_cast<const unsigned char*>(bitmap.constData()) + bitmap.size())
    };

}

#ifdef Q_OS_WIN
#include <windows.h>
#include <chrono>
#include <thread>
#include "signoTec/STPadLib.h"

PatientSignature PatientSigner::signWithSignotec(const std::string& what)
{
    PatientSignature ps; 

    if (STDeviceOpen(0, TRUE) < 0)         
        return ps;

    STSensorSetSignRect(0, 0, 0, 0);

    if (STSignatureStart() < 0) goto closePad;
    while (STSignatureGetState())
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    STSignatureConfirm();

    {
        LONG sz = 0;
        STSignatureGetSignData(nullptr, &sz);
        std::vector<unsigned char> buf(sz);
        if (STSignatureGetSignData(buf.data(), &sz) < 0) goto closePad;
        ps.signatureObject = (buf.data(), sz);
    }


 /*   {
        LONG sz = 0;
        const LONG kPNG = 2;
        const LONG kTrans = 1;                      
        STSignatureSaveAsStreamEx(kPNG, nullptr, &sz, kTrans);
        ps.sigImage.resize(sz);
        if (STSignatureSaveAsStreamEx(kPNG,
            ps.sigImage.data(), &sz, kTrans) < 0)
            goto closePad;
    }
    */

    {
        LONG sz = 0;
        std::vector<unsigned char> cert(sz);
        if (STRSASaveSigningCertAsStream(cert.data(), &sz, CERTTYPE::kCert_DER) == 0)
            ps.signatureCertificate = (cert.data(), sz);
    }
/*
    {
        BYTE hash[32];
        // any SHA-256 routine … 
        // e.g. BCrypt, OpenSSL, std::crypto (C++26), …

        STRSASetHash(hash, kSha256, 0, 0); 
        if (STRSASign(nullptr, &psigSize, kPkcs1_v1_5, 0) == 0)
        {
            LONG psigSize = 0;
            STRSASign(nullptr, &psigSize, kPkcs1_v1_5, 0);   
            std::vector<unsigned char> sig(psigSize);
            if (STRSASign(sig.data(), &psigSize, kPkcs1_v1_5, 0) == 0)
                ps.signature = base64Encode(sig.data(), psigSize);
        }
    }
    */
closePad:
    STDeviceClose(0);
    STControlExit();                                
    return ps;                                       
}

#else
PatientSignature PatientSigner::signWithSignotec(const std::string& what)
{
	return {};
}

#endif