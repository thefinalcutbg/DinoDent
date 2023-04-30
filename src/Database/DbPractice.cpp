#include "DbPractice.h"
#include "Database.h"
#include "Model/Parser.h"

Practice DbPractice::getPractice(const std::string rziCode)
{
    std::string query = "SELECT rzi, name, bulstat, firm_address, practice_address, legal_entity, pass, vat, nhif_contract, settings "
        "FROM practice WHERE rzi = '" + rziCode + "'";

    Practice practice;

    for (Db db(query); db.hasRows();)
    {
        practice.rziCode = db.asString(0);
        practice.name = db.asString(1);
        practice.bulstat = db.asString(2);
        practice.firm_address = db.asString(3);
        practice.practice_address = db.asInt(4);
        practice.legal_entity = db.asInt(5);
        practice.pass = db.asString(6);
        practice.vat = db.asString(7);
        practice.nhif_contract = Parser::parseContract(db.asString(8));
        practice.settings = Parser::parseSettings(db.asString(9));

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
        "practice_address = '" + practice.practice_address.ekatte() + "', "
        "legal_entity = '" + std::to_string(practice.legal_entity) + "', "
        "vat = '" + practice.vat + "', "
        "pass = '" + practice.pass + "', "
        "nhif_contract = '" + Parser::write(practice.nhif_contract) + "', "
        "settings = '" + Parser::write(practice.settings) + "' "
        "WHERE rzi = '" + currentRZI + "' "

    );
}

#include "Resources.h"

void DbPractice::insertPractice(const Practice& practice)
{

    Db::crudQuery(

        "INSERT INTO practice "
        "(rzi, name, bulstat, firm_address, practice_address, pass, legal_entity, vat, nhif_contract, priceList, settings) "
        "VALUES("
        "'" + practice.rziCode + "', "
        "'" + practice.name + "', "
        "'" + practice.bulstat + "', "
        "'" + practice.firm_address + "', "
        "'" + practice.practice_address.ekatte() + "', "
        "'" + practice.pass + "', "
        "'" + std::to_string(practice.legal_entity) + "', "
        "'" + practice.vat + "', "
        "'" + Parser::write(practice.nhif_contract) + "', "
        "'" + Resources::defaultPriceListJson() + "', "
        "'" + Parser::write(practice.settings) + "' "
        ")"

    );
}

bool DbPractice::deletePractice(const std::string& rziCode)
{
    Db db("DELETE FROM practice WHERE practice.rzi=?");

    db.bind(1, rziCode);

    return db.execute();
}

std::vector<PracticePair> DbPractice::getPracticeList(const std::string& doctorLPK)
{
    std::string query =
        "SELECT practice.rzi, practice.name, practice.pass FROM practice";
        
        if (doctorLPK.size()) {
            query += 
                " LEFT JOIN practice_doctor ON "
                "practice.rzi = practice_doctor.practice_rzi "
                "WHERE practice_doctor.doctor_lpk = '" + doctorLPK + "'"
                ;
        }

       query += " ORDER BY practice.name ASC";

    std::vector<PracticePair> practiceList;

    for (Db db(query); db.hasRows();)
    {
        practiceList.emplace_back(
            PracticePair{
                db.asString(0),
                db.asString(1),
                db.asString(2)
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
            + std::to_string(doc.admin) + ", "
            + std::to_string(static_cast<int>(doc.specialty)) + ")"
        );
    }
}

std::vector<PracticeDoctor> DbPractice::getDoctors(const std::string& practiceRZI)
{
    std::vector<PracticeDoctor> result;

    std::string query = "SELECT practice_doctor.doctor_lpk, doctor.fname, doctor.lname, practice_doctor.admin, practice_doctor.specialty "
        "FROM practice_doctor LEFT JOIN doctor ON practice_doctor.doctor_lpk = doctor.lpk "
        "WHERE practice_doctor.practice_rzi = '" + practiceRZI + "'";

    for(Db db(query); db.hasRows();)
    {
        result.emplace_back(
            PracticeDoctor{
                db.asString(0),
                doctorPrefix + db.asString(1) + " " + db.asString(2),
                db.asBool(3), 
                static_cast<NhifSpecialty>(db.asInt(4))
            }
        );
    }

    return result;
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
