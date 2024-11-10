#include "DbPractice.h"
#include "Database.h"
#include "Model/Parser.h"

Practice DbPractice::getPractice(const std::string rziCode)
{
    std::string query = "SELECT rzi, name, bulstat, firm_address, practice_address, legal_entity, pass, vat, nhif_contract, settings, self_insured_id, bank, iban, bic "
        "FROM practice WHERE rzi = '" + rziCode + "'";

    Practice practice;

    Db db;

    db.newStatement(query);

    while (db.hasRows())
    {
        practice.rziCode = db.asString(0);
        practice.name = db.asString(1);
        practice.bulstat = db.asString(2);
        practice.firm_address = db.asString(3);
        practice.practice_address = db.asInt(4);
        practice.legal_entity = db.asInt(5);
        practice.pass = db.asString(6);
        practice.hasVat = db.asBool(7);
        practice.nhif_contract = Parser::parseContract(db.asString(8));
        practice.settings = Parser::parseSettings(db.asString(9));
        practice.selfInsuredId = db.asString(10);
        practice.bank = db.asString(11);
        practice.iban = db.asString(12);
        practice.bic = db.asString(13);

    }

    return practice;
}

bool DbPractice::updatePractice(const Practice& practice, const std::string& currentRZI)
{

    Db db(
        "UPDATE practice SET "
        "name=?,"
        "bulstat=?,"
        "firm_address=?,"
        "practice_address=?,"
        "legal_entity=?,"
        "vat=?,"
        "pass=?,"
        "nhif_contract=?,"
        "self_insured_id=?, "
        "bank=?, "
        "iban=?, "
        "bic=? "
        "WHERE rzi=?"
    );

    db.bind(1, practice.name);
    db.bind(2, practice.bulstat);
    db.bind(3, practice.firm_address);
    db.bind(4, practice.practice_address.getIdxAsInt());
    db.bind(5, practice.legal_entity);
    db.bind(6, practice.hasVat);
    db.bind(7, practice.pass);
    db.bind(8, Parser::write(practice.nhif_contract));
    db.bind(9, practice.selfInsuredId);
    db.bind(10, practice.bank);
    db.bind(11, practice.iban);
    db.bind(12, practice.bic);
    db.bind(13, currentRZI);

    if (!db.execute()) return false;

    //optimization
    if (practice.rziCode != currentRZI) {
        db.newStatement("UPDATE practice SET rzi=? WHERE rzi=?");
        db.bind(1, practice.rziCode);
        db.bind(2, currentRZI);

        if (!db.execute()) return false;
    }

    return true;

}

bool DbPractice::updatePracticeSettings(const Settings& s, const std::string& rzi)
{
    Db db(
        "UPDATE practice SET "
        "settings=? "
        "WHERE rzi=?"
    );


    db.bind(1, Parser::write(s));
    db.bind(2, rzi);

    return db.execute();
}

bool DbPractice::insertPractice(const Practice& practice)
{

    Db db(
        "INSERT INTO practice "
        "(rzi, name, bulstat, firm_address, practice_address, pass, legal_entity, vat, nhif_contract, settings, self_insured_id, bank, iban, bic) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
    );

    db.bind(1, practice.rziCode);
    db.bind(2, practice.name);
    db.bind(3, practice.bulstat);
    db.bind(4, practice.firm_address);
    db.bind(5, practice.practice_address.getIdxAsInt());
    db.bind(6, practice.pass);
    db.bind(7, std::to_string(practice.legal_entity));
    db.bind(8, practice.hasVat);
    db.bind(9, Parser::write(practice.nhif_contract));
    db.bind(10, Parser::write(practice.settings));
    db.bind(11, practice.selfInsuredId);
    db.bind(12, practice.bank);
    db.bind(13, practice.iban);
    db.bind(14, practice.bic);

    return db.execute();

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

bool DbPractice::setDoctorsPracticeList(std::vector<PracticeDoctor> doctors, const std::string& practiceRZI)
{
    std::string query = "DELETE FROM practice_doctor "
        "WHERE practice_rzi = '" + practiceRZI + "'";

    Db db;

    db.execute(query);

    size_t insertCount{ 0 };

    for (auto& doc : doctors)
    {
       insertCount += 
           db.execute(
            "INSERT INTO practice_doctor "
            "VALUES ('"
            + practiceRZI + "', '"
            + doc.lpk + "', "
            + std::to_string(doc.admin) + ", "
            + std::to_string(static_cast<int>(doc.specialty)) + ")"
        );
    }

    return insertCount == doctors.size();
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
