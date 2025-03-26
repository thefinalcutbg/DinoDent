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

    const auto filepath = result.absoluteFilePath(filename.c_str());

    QFileInfo check_file(filepath);

    if (!check_file.exists()) return filepath.toStdString();

    auto answer = ModalDialogBuilder::YesNoCancelDailog(
        "Този документ вече съществува. Искате ли  да го презапишете? "
        "Ако изберете 'Не', ще бъде генериран нов PDF файл."
    );

    if (answer == DialogAnswer::Cancel) return std::string();

    if (answer == DialogAnswer::Yes) return filepath.toStdString();

    //creating new name with (num) suffix

    int counter = 0;

    auto newFilepath = filepath;

   qsizetype insertPosition = filepath.lastIndexOf(".");

    while (QFileInfo().exists(newFilepath)) {

        counter++;

        newFilepath = filepath;

        newFilepath.insert(insertPosition, "_"+QString::number(counter));
    }
        
    return newFilepath.toStdString();

}   

std::string getPath(const Patient& p, const Date& date, const std::string& docTypeStr, const std::string& filename) {

	std::array<std::string, 6> dirStr;

	dirStr[DirType::PRACTICE] = User::practice().rziCode;
	dirStr[DirType::DOCTOR] = User::doctor().LPK;
	dirStr[DirType::PATIENTLF] = FreeFn::toUpper(p.LastName) + " " + p.FirstName + " " + p.id.substr(0,6);
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

std::string FilePaths::get(const AmbList& amb, const Patient& patient, bool nhifForm)
{
    if (amb.nrn.empty()) return {};

    std::string docType = nhifForm ? "_AMB_NHIF_" : "_AMB_";
 
    std::string filename = amb.nrn + docType + patient.id.substr(0,6) + ".pdf";

    return getPath(patient, amb.date, "Амбулаторни листове", filename);
} 

std::string FilePaths::get(const Prescription& prescr, const Patient& patient)
{
    if (prescr.NRN.empty()) return {};

    auto filename = prescr.NRN + "_PRESCR_" + patient.id.substr(0, 6) + ".pdf";

    return getPath(patient, prescr.date, "Рецепти", filename);
}

std::string FilePaths::get(const Invoice& invoice)
{
    std::string subdir = User::practice().bulstat.c_str();
    subdir += "-";
    subdir += "Фактури";
    subdir += "/";
    subdir += invoice.date.to8601().substr(0, 7);

    auto filename = FreeFn::leadZeroes(invoice.number, 10);
    filename += "-"; 
    filename += invoice.recipient.name;
    filename += ".pdf";
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
        "_" + declStrShort[declType] + "_" + 
        patient.id.substr(0,6) + 
        ".pdf";

    return getPath(patient, Date::currentDate(), declStr[declType], filename);
}
