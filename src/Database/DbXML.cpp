#include "DbXML.h"
#include "Model/CityCode.h"
#include "Model/Parser/Parser.h"
#include "Model/Tooth/ToothUtils.h"
#include "Model/User/UserManager.h"

std::string leadZeroes(int num, int totalLength)
{
    std::string formated;
    formated.reserve(totalLength);

    std::string number(std::to_string(num));

    for (int i = 0; i < totalLength - number.length(); i++)
        formated += "0";

    formated += number;

    return formated;
}

int getSheetIdx(const std::vector<AmbListXML>& sheets, const std::string& amblist_id)
{
    for (int i = 0; i<sheets.size(); i++){
        if (sheets[i].id == amblist_id){
            return i;
        }
    }
    return -1;
}

std::string getSpecType(bool fullCoverage)
{
    constexpr const char* specType[4]
    {
        "PRIMARY NORM", //код специалнсот 60 или 64 и цената се покрива частично/изцяло от НЗОК
        "PRIMARY SPEC", //код специалнсот 60 или 64 и цената се покрива ИЗЦЯЛО от НЗОК
        "SPEC NORM",    //код специалнсот 61, 62 или 68 и цената се покрива частично/изцяло от НЗОК
        "SPEC DOMOVE"   //код специалнсот 61, 62 или 68 и цената се покрива ИЗЦЯЛО от НЗОК
        //"SPEC_PS      //за обща анестезия (такъв няма feature все още)
    };

    auto& specialty = UserManager::currentUser().specialty;

    bool primaryDentalAid = specialty == 60 || specialty == 64;

    if (!fullCoverage && primaryDentalAid)
        return specType[0];

    if (fullCoverage && primaryDentalAid)
        return specType[1];

    if (!fullCoverage && !primaryDentalAid)
        return specType[2];

    if (!fullCoverage && primaryDentalAid)
        return specType[3];
}

std::string getAllergiesAndStuff(const std::string& allergiesAndStuff)
{
    if (!allergiesAndStuff.size())
        return u8"Не съобщава";
    return allergiesAndStuff;
}

std::vector<AmbListXML> DbXML::getAmbListXML(int month, int year, std::string RZICode, std::string LPK)
{
    openConnection();

    std::string query =
        "SELECT "
        "amblist.id,"                   //0
        "patient.type,"                 //1
        "patient.id,"                   //2
        "patient.city,"                 //3
        "patient.fname,"                //4
        "patient.mname,"                //5
        "patient.lname,"                //6
        "amblist.fullCoverage,"         //7
        "amblist.num,"                  //8
        "patient.hirbno,"               //9
        "patient.allergies,"            //10
        "patient.pastDiseases,"         //11
        "patient.currentDiseases,"      //12
        "amblist.status_json "          //13
        "FROM amblist "
        "LEFT JOIN patient ON amblist.patient_id = patient.id "
        "LEFT JOIN procedure ON amblist.id = procedure.amblist_id "
        "GROUP BY amblist.id "
        "HAVING "
            "amblist.month = " + std::to_string(month) + " "
        "AND amblist.year = " + std::to_string(year) + " "
        "AND sum(procedure.nzok) > 0 "
        "AND amblist.rzi = '" + RZICode + "' "
        "AND amblist.lpk = '" + LPK + "' "
        "ORDER BY amblist.num";

        sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

        std::vector<AmbListXML> report;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        auto [RHIF, HealthRegion] = CityCode::getCodes(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));

        report.emplace_back();

        auto& a = report.back();

        a.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            a.personType = sqlite3_column_int(stmt, 1);
            a.personIdentifier = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

            a.RHIF = RHIF;
            a.HealthRegion = HealthRegion;

            a.personFirstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            a.personMiddleName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            a.personLastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));

            a.specificationType = getSpecType(static_cast<bool>(sqlite3_column_int(stmt, 7)));
            a.ambulatorySheetNo = leadZeroes(sqlite3_column_int(stmt, 8), 6);
            a.HIRBNo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));

            a.allergies = getAllergiesAndStuff(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10)));
            a.pastDiseases = getAllergiesAndStuff(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11)));
            a.currentDiseases = getAllergiesAndStuff(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12)));
            a.substitute = 0,
            a.sign = 1,

            a.teeth = Parser::getTeethXML(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13)));

        
    }


    sqlite3_finalize(stmt);

    query = "SELECT "
            "procedure.amblist_id,"     //0
            "procedure.day,"            //1
            "amblist.month,"            //2
            "amblist.year,"             //3
            "procedure.data,"           //4
            "procedure.tooth, "         //5
            "procedure.temp, "          //6
            "procedure.code "           //7
            "FROM procedure "
            "LEFT JOIN amblist ON amblist.id = procedure.amblist_id "
            "WHERE "
            "amblist.month = " + std::to_string(month) + " "
            "AND amblist.year = " + std::to_string(year) + " "
            "AND procedure.nzok = 1 "
            "AND amblist.rzi = '" + RZICode + "' "
            "AND amblist.lpk = '" + LPK + "' "
            "ORDER BY amblist.num ASC, procedure.seq ASC";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        auto& currentSheet = 
            report[getSheetIdx(report, reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)))];


        currentSheet.services.emplace_back(
            ServiceXML{

                Date{
                    sqlite3_column_int(stmt, 1),
                    sqlite3_column_int(stmt, 2),
                    sqlite3_column_int(stmt, 3)
                },

                Parser::parseDiagnosis(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))),

                ToothUtils::getToothNumber(sqlite3_column_int(stmt, 5), static_cast<bool>(sqlite3_column_int(stmt, 6))),

                sqlite3_column_int(stmt, 7)
            }
            );

    }

    sqlite3_finalize(stmt);

    closeConnection();

    return report;
}
