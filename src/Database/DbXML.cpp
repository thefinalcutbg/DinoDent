#include "DbXML.h"
#include "Model/CityCode.h"
#include "Model/Parser/Parser.h"
#include "Model/Tooth/ToothUtils.h"
#include "Model/User/UserManager.h"
#include "Database/Database.h"

int getSheetIdx(const std::vector<AmbListXML>& sheets, long long amb_rowid)
{

    for (int i = 0; i<sheets.size(); i++){
        if (sheets[i].rowid == amb_rowid){
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

    auto& specialty = UserManager::currentUser().doctor.specialty;

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

    std::string query =
        "SELECT "
        "amblist.rowid,"                   //0
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
        "amblist.status_json, "         //13
        "patient.birth "                //14
        "FROM amblist "
        "LEFT JOIN patient ON amblist.patient_rowid = patient.rowid "
        "LEFT JOIN procedure ON amblist.rowid = procedure.amblist_rowid "
        "GROUP BY amblist.rowid "
        "HAVING "
            "amblist.month = " + std::to_string(month) + " "
        "AND amblist.year = " + std::to_string(year) + " "
        "AND sum(procedure.nzok) > 0 "
        "AND amblist.rzi = '" + RZICode + "' "
        "AND amblist.lpk = '" + LPK + "' "
        "ORDER BY amblist.num";

        std::vector<AmbListXML> report;

    Db db(query);

    while (db.hasRows())
    {
        auto [RHIF, HealthRegion] = CityCode::getCodes(CityCode::cityFromIndex(db.asInt(3)));

        report.emplace_back();

        auto& a = report.back();

            a.rowid = db.asRowId(0);
            a.personType = db.asInt(1);
            a.personIdentifier = db.asString(2);

            a.RHIF = RHIF;
            a.HealthRegion = HealthRegion;

            a.personFirstName = db.asString(4);
            a.personMiddleName = db.asString(5);
            a.personLastName = db.asString(6);

            a.specificationType = getSpecType(db.asInt(7));
            a.ambulatorySheetNo = db.asInt(8);
            a.HIRBNo = db.asString(9);

            a.allergies = getAllergiesAndStuff(db.asString(10));
            a.pastDiseases = getAllergiesAndStuff(db.asString(11));
            a.currentDiseases = getAllergiesAndStuff(db.asString(12));
            a.substitute = 0,
            a.sign = 1,

            a.teeth = Parser::getTeethXML(db.asString(13));
            a.birthDate = Date(db.asString(14));

        
    }


    query = "SELECT "
            "procedure.amblist_rowid,"     //0
            "procedure.day,"            //1
            "amblist.month,"            //2
            "amblist.year,"             //3
            "procedure.data,"           //4
            "procedure.tooth, "         //5
            "procedure.deciduous, "     //6
            "procedure.code "           //7
            "FROM procedure "
            "LEFT JOIN amblist ON amblist.rowid = procedure.amblist_rowid "
            "WHERE "
            "amblist.month = " + std::to_string(month) + " "
            "AND amblist.year = " + std::to_string(year) + " "
            "AND procedure.nzok = 1 "
            "AND amblist.rzi = '" + RZICode + "' "
            "AND amblist.lpk = '" + LPK + "' "
            "ORDER BY amblist.num ASC, procedure.rowid ASC";

    db.newStatement(query);

    while (db.hasRows())
    {
        auto& currentSheet = 
            report[getSheetIdx(report, db.asRowId(0))];


        currentSheet.services.emplace_back(
            ServiceXML{

                Date{
                    db.asInt(1),
                    db.asInt(2),
                    db.asInt(3)
                },

                Parser::parseDiagnosis(db.asString(4)),

                ToothUtils::getToothNumber(db.asInt(5), static_cast<bool>(db.asInt(6))),

                db.asInt(7)
            }
            );

    }

    return report;
}
