#include "PatientSigner.h"
#include "crypto.h"
#include <QAxWidget>
#include <QAxObject>
#include "Model/Patient.h"

std::string signWithWacom(const std::string& what) {

    const QString licence = "eyJhbGciOiJSUzUxMiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI3YmM5Y2IxYWIxMGE0NmUxODI2N2E5MTJkYTA2ZTI3NiIsImV4cCI6MjE0NzQ4MzY0NywiaWF0IjoxNTYwOTUwMjcyLCJyaWdodHMiOlsiU0lHX1NES19DT1JFIiwiU0lHQ0FQVFhfQUNDRVNTIl0sImRldmljZXMiOlsiV0FDT01fQU5ZIl0sInR5cGUiOiJwcm9kIiwibGljX25hbWUiOiJTaWduYXR1cmUgU0RLIiwid2Fjb21faWQiOiI3YmM5Y2IxYWIxMGE0NmUxODI2N2E5MTJkYTA2ZTI3NiIsImxpY191aWQiOiJiODUyM2ViYi0xOGI3LTQ3OGEtYTlkZS04NDlmZTIyNmIwMDIiLCJhcHBzX3dpbmRvd3MiOltdLCJhcHBzX2lvcyI6W10sImFwcHNfYW5kcm9pZCI6W10sIm1hY2hpbmVfaWRzIjpbXX0.ONy3iYQ7lC6rQhou7rz4iJT_OJ20087gWz7GtCgYX3uNtKjmnEaNuP3QkjgxOK_vgOrTdwzD-nm-ysiTDs2GcPlOdUPErSp_bcX8kFBZVmGLyJtmeInAW6HuSp2-57ngoGFivTH_l1kkQ1KMvzDKHJbRglsPpd4nVHhx9WkvqczXyogldygvl0LRidyPOsS5H2GYmaPiyIp9In6meqeNQ1n9zkxSHo7B11mp_WXJXl0k1pek7py8XYCedCNW5qnLi4UCNlfTd6Mk9qz31arsiWsesPeR9PN121LBJtiPi023yQU8mgb9piw_a-ccciviJuNsEuRDN3sGnqONG3dMSA";

    QAxWidget ctl("Florentis.SigCtl");
    ctl.setProperty("Licence", licence);

    QAxObject dyn("Florentis.DynamicCapture");
    dyn.setProperty("Licence", licence);

    QAxObject hash("Florentis.Hash");
    hash.setProperty("Type", 4);
    hash.dynamicCall("Add(Data)", what.data());

    QString who = "blah";
	QString why = "blah";

    QVariant resultVariant = dyn.dynamicCall("Capture(SigCtl, Who, Why, What, Key)",
        ctl.asVariant(),
        who, why, what.data()
        //hash.asVariant()
    );

    int result = resultVariant.toInt();

    if (result != 0) return std::string();

    return ctl.property("InputSignature").toString().toStdString();

}

std::string PatientSigner::sign(const std::string& xmlContents)
{
    return signWithWacom(xmlContents);
}
