#include "DirTree.h"

#include <QDir>
#include <QStandardPaths>

#include "Model/User.h"
#include "Model/Dental/AmbList.h"
#include "Model/Prescription/Prescription.h"
#include "Model/Financial/Invoice.h"

#include "Model/TabletSettings.h"
#include "GlobalSettings.h"
#include "View/ModalDialogBuilder.h"

std::string getPath(const std::string& subdir, const std::string& filename)
{
    auto settings = GlobalSettings::getTabletSettings();

    QDir result(settings.pdfDir.c_str());

    bool pathExists = result.mkpath(subdir.c_str());

    if (!pathExists) {
        return "";
    }

    result.cd(subdir.c_str());

    auto filepath = result.absoluteFilePath(filename.c_str());

    QFileInfo check_file(filepath);

    if (check_file.exists() &&
        !ModalDialogBuilder::askDialog(
            "Този файл вече съществува. Сигурни ли"
            " сте че ""искате да го презапишете?"
        )
        )
    {
        return std::string();
    }

    return filepath.toStdString();

}   

std::string getPath(const Patient& p, const Date& date, const std::string& docTypeStr, const std::string& filename) {

	std::array<std::string, 6> dirStr;

	dirStr[TabletSettings::DirType::PRACTICE] = User::practice().rziCode + "-" + User::practice().name;
	dirStr[TabletSettings::DirType::DOCTOR] = User::doctor().LPK + "-" + User::doctor().getFullName(false);
	dirStr[TabletSettings::DirType::PATIENTLF] = FreeFn::toUpper(p.LastName) + " " + p.FirstName + "-" + p.id.substr(0,6);
	dirStr[TabletSettings::DirType::PATIENTFL] = p.FirstName + " " + p.LastName + " " + p.id.substr(0, 6);
	dirStr[TabletSettings::DirType::YEARMONTH] = date.to8601().substr(0, 7);
	dirStr[TabletSettings::DirType::DOCTYPE] = docTypeStr;

    auto settings = GlobalSettings::getTabletSettings();

    std::string subdir;

    for (auto& dir : settings.subdirStructure) {
        subdir += dirStr[dir].c_str();
        subdir += "/";
    }

    return getPath(subdir, filename);

  
}

std::string DirTree::get(const AmbList& amb, const Patient& patient)
{
    if (amb.nrn.empty()) return {};

    auto filename = amb.nrn + "-AMB-" + patient.id.substr(0,6) + ".pdf";

    return getPath(patient, amb.date, "Амбулаторни листове", filename);
}

std::string DirTree::get(const Prescription& prescr, const Patient& patient)
{
    if (prescr.NRN.empty()) return {};

    auto filename = prescr.NRN + "-PRESCR-" + patient.id.substr(0, 6) + ".pdf";

    return getPath(patient, prescr.date, "Рецепти", filename);
}

std::string DirTree::get(const Invoice& invoice)
{
    auto settings = GlobalSettings::getTabletSettings();

    std::string subdir = "ФАКТУРИ- ";
    subdir += User::practice().bulstat.c_str();
    subdir += "/";
    subdir += invoice.date.to8601().substr(0, 7);

    auto filename = FreeFn::leadZeroes(invoice.number, 10);
    filename += "-"; 
    filename + invoice.recipient.name;

    return getPath(subdir, filename);

}

std::string DirTree::get(DeclarationType declType, const Patient& patient)
{
    const std::string declStr[] = {
        "Декларации за тотални протези НЗОК",
        "Декларации за активна здравна книжка",
        "Информирани съгласия",
        "Декларации за защита на лични данни"
    };

    const std::string declStrShort[] = {
        "DENTURE",
        "HIRBNo",
        "CONSENT",
        "GDPR"
    };

    auto filename = 
        Date::currentDate().to8601() + 
        "-" + declStrShort[declType] + 
        patient.id.substr(0,6) + 
        ".pdf";

    return getPath(patient, Date::currentDate(), declStr[declType], filename);
}
