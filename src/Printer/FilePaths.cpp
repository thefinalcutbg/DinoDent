#include "FilePaths.h"

#include <QDir>
#include <QStandardPaths>

#include "Model/User.h"
#include "Model/Dental/AmbList.h"
#include "Model/Prescription/Prescription.h"
#include "Model/Financial/Invoice.h"

#include "View/ModalDialogBuilder.h"

std::vector<DirType> subdir_structure;
std::string s_dirLocation;

std::string getPath(const std::string& subdir, const std::string& filename)
{
    QDir result(s_dirLocation.c_str());

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

	dirStr[DirType::PRACTICE] = User::practice().rziCode + "-" + User::practice().name;
	dirStr[DirType::DOCTOR] = User::doctor().LPK + "-" + User::doctor().getFullName(false);
	dirStr[DirType::PATIENTLF] = FreeFn::toUpper(p.LastName) + " " + p.FirstName + "-" + p.id.substr(0,6);
	dirStr[DirType::PATIENTFL] = p.FirstName + " " + p.LastName + " " + p.id.substr(0, 6);
	dirStr[DirType::YEARMONTH] = date.to8601().substr(0, 7);
	dirStr[DirType::DOCTYPE] = docTypeStr;


    std::string subdir;

    for (auto& dir : subdir_structure) {
        subdir += dirStr[dir].c_str();
        subdir += "/";
    }

    return getPath(subdir, filename);

  
}

void FilePaths::setSettings(const std::string& dir, const std::vector<DirType> subdirStructure)
{
    s_dirLocation = dir;
    subdir_structure = subdirStructure;
}

std::string FilePaths::get(const AmbList& amb, const Patient& patient)
{
    if (amb.nrn.empty()) return {};

    auto filename = amb.nrn + "-AMB-" + patient.id.substr(0,6) + ".pdf";

    return getPath(patient, amb.date, "Амбулаторни листове", filename);
}

std::string FilePaths::get(const Prescription& prescr, const Patient& patient)
{
    if (prescr.NRN.empty()) return {};

    auto filename = prescr.NRN + "-PRESCR-" + patient.id.substr(0, 6) + ".pdf";

    return getPath(patient, prescr.date, "Рецепти", filename);
}

std::string FilePaths::get(const Invoice& invoice)
{
    std::string subdir = "ФАКТУРИ- ";
    subdir += User::practice().bulstat.c_str();
    subdir += "/";
    subdir += invoice.date.to8601().substr(0, 7);

    auto filename = FreeFn::leadZeroes(invoice.number, 10);
    filename += "-"; 
    filename + invoice.recipient.name;

    return getPath(subdir, filename);

}

std::string FilePaths::get(DeclarationType declType, const Patient& patient)
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
