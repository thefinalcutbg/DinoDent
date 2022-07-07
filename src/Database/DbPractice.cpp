#include "DbPractice.h"
#include "Database.h"
#include "Model/Parser/Parser.h"
#include <fstream>

Practice DbPractice::getPractice(const std::string rziCode)
{
    std::string query = "SELECT rzi, name, bulstat, firm_address, practice_address, legal_entity, pass, vat, nzok_contract, priceList, settings "
        "FROM practice WHERE rzi = '" + rziCode + "'";

    Practice practice;

    for (Db db(query); db.hasRows();)
    {
        practice.rziCode = db.asString(0);
        practice.name = db.asString(1);
        practice.bulstat = db.asString(2);
        practice.firm_address = db.asString(3);
        practice.practice_address = db.asString(4);
        practice.legal_entity = db.asInt(5);
        practice.pass = db.asString(6);
        practice.vat = db.asString(7);
        practice.nzok_contract = Parser::parseContract(db.asString(8));
        practice.priceList = Parser::getPriceList(db.asString(9));
        practice.settings = Parser::parseSettings(db.asString(10));

    }


    return practice;
}

void DbPractice::updatePractice(const Practice& practice, const std::string& currentRZI)
{
    Db::crudQuery(

        "UPDATE practice SET "
        "rzi = '" + practice.rziCode + "', "
        "name = '" + practice.name + "', "
        "bulstat = '" + practice.bulstat + "', "
        "firm_address = '" + practice.firm_address + "', "
        "practice_address = '" + practice.practice_address + "', "
        "legal_entity = '" + std::to_string(practice.legal_entity) + "', "
        "vat = '" + practice.vat + "', "
        "pass = '" + practice.pass + "', "
        "nzok_contract = '" + Parser::write(practice.nzok_contract) + "', "
        "settings = '" + Parser::write(practice.settings) + "' "
        "WHERE rzi = '" + currentRZI + "' "

    );
}

void DbPractice::insertPractice(const Practice& practice)
{
    std::string line, text;
    std::ifstream in("data/defaultPriceList.json");
    while (std::getline(in, line))
    {
        text += line;
    }

    Db::crudQuery(

        "INSERT INTO practice "
        "(rzi, name, bulstat, firm_address, practice_address, pass, legal_entity, vat, nzok_contract, priceList, settings) "
        "VALUES("
        "'" + practice.rziCode + "', "
        "'" + practice.name + "', "
        "'" + practice.bulstat + "', "
        "'" + practice.firm_address + "', "
        "'" + practice.practice_address + "', "
        "'" + practice.pass + "', "
        "'" + std::to_string(practice.legal_entity) + "', "
        "'" + practice.vat + "', "
        "'" + Parser::write(practice.nzok_contract) + "', "
        "'" + text + "', "
        "'" + Parser::write(practice.settings) + "' "
        ")"

    );
}

std::vector<PracticePair> DbPractice::getPracticeList(const std::string& doctorLPK)
{
    std::string query =
        "SELECT practice.rzi, practice.name "
        "FROM practice LEFT JOIN practice_doctor ON "
        "practice.rzi = practice_doctor.practice_rzi "
        "WHERE practice_doctor.doctor_lpk = '" + doctorLPK + "' "
        "ORDER BY name ASC";

    std::vector<PracticePair> practiceList;

    for (Db db(query); db.hasRows();)
    {
        practiceList.emplace_back(
            PracticePair{
                db.asString(0),
                db.asString(1)
            }
        );
    }

    return practiceList;
}

void DbPractice::setDoctorsPracticeList(std::vector<PracticeDoctor> doctors, const std::string& practiceRZI)
{
    std::string query = "DELETE FROM practice_doctor "
        "WHERE practice_rzi = '" + practiceRZI + "'";

    Db db;

    db.execute(query);

    for (auto& doc : doctors)
    {
        db.execute(
            "INSERT INTO practice_doctor "
            "VALUES ('"
            + practiceRZI + "', '"
            + doc.lpk + "', "
            + std::to_string(doc.admin) + ")"
        );
    }
}

std::vector<PracticeDoctor> DbPractice::getDoctors(const std::string& practiceRZI)
{
    std::vector<PracticeDoctor> result;

    std::string query = "SELECT practice_doctor.doctor_lpk, doctor.fname, doctor.lname, practice_doctor.admin "
        "FROM practice_doctor LEFT JOIN doctor ON practice_doctor.doctor_lpk = doctor.lpk "
        "WHERE practice_doctor.practice_rzi = '" + practiceRZI + "'";

    for(Db db(query); db.hasRows();)
    {
        result.emplace_back(
            PracticeDoctor{
                db.asString(0),
                doctorPrefix + db.asString(1) + " " + db.asString(2),
                static_cast<bool>(db.asInt(3))
            }
        );
    }

    return result;
}

void DbPractice::updatePriceList(const std::vector<ProcedureTemplate>& priceList, const std::string& rziCode)
{
    Db::crudQuery(
        "UPDATE practice SET "
        "priceList = '" + Parser::write(priceList) + "' "
        "WHERE rzi = '" + rziCode + "'"
    );
}

bool DbPractice::practiceExists(const std::string& rzi)
{

    bool exists{ false };

    std::string query = "SELECT COUNT(*) "
        "FROM practice "
        "WHERE rzi ='" + rzi + "' ";

    for (Db db(query); db.hasRows();){
        return db.asInt(0);
    }

    return false;
}

bool DbPractice::noPractices()
{
    std::string query = "SELECT COUNT(*) FROM practice";

    for (Db db(query); db.hasRows();){
        return !db.asInt(0);
    }

    return true;
}
