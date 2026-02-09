#include "DbPractice.h"
#include "Database.h"
#include "Model/Parser.h"

Practice DbPractice::getPractice(const std::string rziCode)
{
    std::string query = "SELECT rzi, name, bulstat, firm_address, practice_address, street_address, legal_entity, pass, vat, nhif_contract, settings, self_insured_id, bank, iban, bic "
        "FROM practice WHERE rzi = ?";

    Practice practice;

    Db db;

    db.newStatement(query);

    db.bind(1, rziCode);

    while (db.hasRows())
    {
        practice.rziCode = db.asString(0);
        practice.name = db.asString(1);
        practice.bulstat = db.asString(2);
        practice.firm_address = db.asString(3);
        practice.practice_address = db.asInt(4);
        practice.street_address = db.asString(5);
        practice.legal_entity = db.asInt(6);
        practice.pass = db.asString(7);
        practice.hasVat = db.asBool(8);
        practice.nhif_contract = Parser::parseContract(db.asString(9));
        practice.settings = Parser::parseSettings(db.asString(10));
        practice.selfInsuredId = db.asString(11);
        practice.bank = db.asString(12);
        practice.iban = db.asString(13);
        practice.bic = db.asString(14);

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
        "street_address=?,"
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
    db.bind(5, practice.street_address);
    db.bind(6, practice.legal_entity);
    db.bind(7, practice.hasVat);
    db.bind(8, practice.pass);
    db.bind(9, Parser::write(practice.nhif_contract));
    db.bind(10, practice.selfInsuredId);
    db.bind(11, practice.bank);
    db.bind(12, practice.iban);
    db.bind(13, practice.bic);
    db.bind(14, currentRZI);

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
        "(rzi, name, bulstat, firm_address, practice_address, street_address, pass, legal_entity, vat, nhif_contract, settings, self_insured_id, bank, iban, bic) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
    );

    db.bind(1, practice.rziCode);
    db.bind(2, practice.name);
    db.bind(3, practice.bulstat);
    db.bind(4, practice.firm_address);
    db.bind(5, practice.practice_address.getIdxAsInt());
    db.bind(6, practice.street_address);
    db.bind(7, practice.pass);
    db.bind(8, std::to_string(practice.legal_entity));
    db.bind(9, practice.hasVat);
    db.bind(10, Parser::write(practice.nhif_contract));
    db.bind(11, Parser::write(practice.settings));
    db.bind(12, practice.selfInsuredId);
    db.bind(13, practice.bank);
    db.bind(14, practice.iban);
    db.bind(15, practice.bic);

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

        if (result.back().specialty >= NhifSpecialty::MaxValue) {
            result.back().specialty = NhifSpecialty::None;
        }
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

std::unordered_map<std::string, std::pair<double, double>> DbPractice::getCodeValues(const std::string& rziCode)
{
    std::unordered_map<std::string, std::pair<double, double>> result;

    Db db;

    db.newStatement("SELECT code, from_value, to_value FROM code_list WHERE rzi=?");

    db.bind(1, rziCode);

    while (db.hasRows()) {
        result[db.asString(0)] = {db.asDouble(1), db.asDouble(2)};
    }

    return result;
}


std::pair<double, double> DbPractice::getCodeValue(const std::string &code, const std::string &rziCode)
{
    Db db;

    db.newStatement("SELECT from_value, to_value FROM code_list WHERE rzi=? AND code=?");

    db.bind(1, rziCode);
    db.bind(2, code);

    while (db.hasRows()) {
        return {db.asDouble(0), db.asDouble(1)};
    }

    return {0,0};
}

bool DbPractice::setCodeValues(const std::string& code, const std::pair<double, double>& priceRange, const std::string& rziCode)
{
    Db db;

    if (priceRange.first == 0 && priceRange.second == 0) {
        db.newStatement("DELETE FROM code_list WHERE code=? AND rzi=?");
        db.bind(1, code);
        db.bind(2, rziCode);
        return db.execute();
    }

    db.newStatement(
        "INSERT INTO code_list(rzi, code, from_value, to_value) VALUES(?,?,?,?) "
        "ON CONFLICT(rzi, code) DO UPDATE SET from_value = excluded.from_value, to_value = excluded.to_value"
        );

    db.bind(1, rziCode);
    db.bind(2, code);
    db.bind(3, priceRange.first);
    db.bind(4, priceRange.second);

    return db.execute();
}

std::set<int> DbPractice::getUnfavEkatte(const std::string &rziCode)
{
    std::string query = "SELECT ekatte FROM unfavourable_condition WHERE rzi=?";

    Db db(query);

    db.bind(1, rziCode);

    std::set<int> result;

    while(db.hasRows()){
        result.insert(db.asInt(0));
    }

    return result;

}

void DbPractice::setUnfavEkatte(const std::set<int> &unfavEkatte, const std::string &rziCode)
{
    Db db;

    db.newStatement("DELETE FROM unfavourable_condition WHERE rzi=?");

    db.bind(1, rziCode);

    db.execute();

    for(auto ekatte : unfavEkatte){
        db.newStatement("INSERT INTO unfavourable_condition (ekatte, rzi) VALUES(?,?)");
        db.bind(1, ekatte);
        db.bind(2, rziCode);
        db.execute();
    }
}
