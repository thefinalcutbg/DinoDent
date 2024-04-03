#include "DbDoctor.h"
#include "Database.h"

std::optional<Doctor> DbDoctor::getDoctor(const std::string& lpk, const std::string& pass)
{
    std::optional<Doctor> result;

    std::string query =
        "SELECT fname, mname, lname, phone, several_rhif, his_specialty, pass FROM doctor "
        "WHERE lpk = ? "
        "AND (pass = ? OR pass = '')";

    Db db(query);

    db.bind(1, lpk);
    db.bind(2, pass);


    while(db.hasRows())
    {
        Doctor doctor;

        doctor.LPK = lpk;
        doctor.fname = db.asString(0);
        doctor.mname = db.asString(1);
        doctor.lname = db.asString(2);
        doctor.phone = db.asString(3);
        doctor.severalRHIF = db.asInt(4);
        doctor.hisSpecialty = db.asInt(5);
        doctor.pass = db.asString(6);

        result = doctor;
    }

    return result;
}

std::optional<Doctor> DbDoctor::getDoctor(const std::string& lpk)
{
    std::optional<Doctor> result;

    std::string query =
        "SELECT fname, mname, lname, phone, several_rhif, pass, his_specialty FROM doctor "
        "WHERE lpk = '" + lpk + "'";

    for (Db db(query); db.hasRows();)
    {
        Doctor doctor;

        doctor.LPK = lpk;
        doctor.fname = db.asString(0);
        doctor.mname = db.asString(1);
        doctor.lname = db.asString(2);
        doctor.phone = db.asString(3);
        doctor.severalRHIF = db.asInt(4);
        doctor.pass = db.asString(5);
        doctor.hisSpecialty = db.asInt(6);


        result = doctor;
    }

    return result;

}

std::pair<std::string, std::string> DbDoctor::getLpkAndPassAutoLogin()
{
    std::string lpk;
    std::string pass;

    Db db("SELECT lpk, pass FROM doctor WHERE auto_login=1");

    while (db.hasRows()) {
        lpk = db.asString(0);
        pass = db.asString(1);
    }

    return { lpk, pass };
}

bool DbDoctor::setAutoLogin(const std::string& lpk, bool remember)
{
    std::string query =
        "UPDATE doctor SET auto_login="
        "CASE WHEN lpk = ? THEN ? ELSE 0 END";


    Db db(query);
    db.bind(1, lpk);
    db.bind(2, remember);
    return db.execute();

}

bool DbDoctor::updateDoctor(const Doctor& doctor, const std::string& currentLPK)
{
   Db db;

   db.newStatement(
       "UPDATE doctor SET "

       "fname=?,"
       "mname=?,"
       "lname=?,"
       "pass=?,"
       "phone=?,"
       "several_rhif=?,"
       "his_specialty=? "
       "WHERE lpk=?"
   );

   db.bind(1, doctor.fname);
   db.bind(2, doctor.mname);
   db.bind(3, doctor.lname);
   db.bind(4, doctor.pass);
   db.bind(5, doctor.phone);
   db.bind(6, doctor.severalRHIF);
   db.bind(7, doctor.hisSpecialty.getIdx());
   db.bind(8, currentLPK);

   if(!db.execute()) return false;

   //optimization
   if (doctor.LPK != currentLPK) {
       db.newStatement("UPDATE doctor SET lpk=? WHERE lpk=?");
       db.bind(1, doctor.LPK);
       db.bind(2, currentLPK);

       if (!db.execute()) return false;
   }

   return true;
}

bool DbDoctor::insertDoctor(const Doctor& doctor)
{
    Db db(
        "INSERT INTO doctor (lpk, fname, mname, lname, pass, phone, several_rhif, auto_login, his_specialty ) "
        "VALUES(?,?,?,?,?,?,?,?,?)"
    );

    db.bind(1, doctor.LPK);
    db.bind(2, doctor.fname);
    db.bind(3, doctor.mname);
    db.bind(4, doctor.lname);
    db.bind(5, doctor.pass);
    db.bind(6, doctor.phone);
    db.bind(7, doctor.severalRHIF);
    db.bind(8, false);
    db.bind(9, doctor.hisSpecialty.getIdx());

    return db.execute();
}

bool DbDoctor::suchDoctorExists(const std::string& LPK)
{
    Db db;
    db.newStatement("SELECT COUNT(*) FROM doctor WHERE lpk=?");
    db.bind(1, LPK);

    while (db.hasRows()) {
        return db.asBool(0);
    }

    return false;
}

std::tuple<bool, int> DbDoctor::getAdminAndSpecialty(const std::string& lpk, const std::string& rzi)
{
    std::string query = "SELECT admin, specialty "
        "FROM practice_doctor "
        "WHERE doctor_lpk ='" + lpk + "' "
        "AND practice_rzi = '" + rzi + "'";

    for(Db db(query); db.hasRows();){
        return { db.asBool(0), db.asInt(1) };
    }

    return { false, 0 };
}

std::unordered_map<std::string, std::string> DbDoctor::getDoctorNames()
{
    std::unordered_map <std::string, std::string> doctorNames;

    std::string query = "SELECT lpk, fname, lname FROM doctor";

    for (Db db(query); db.hasRows();)
    {
        doctorNames[db.asString(0)] = doctorPrefix + db.asString(1)+ " " + db.asString(2);
    }

    return doctorNames;
}

bool DbDoctor::updateFavouriteProcedures(const std::vector<std::string>& procedureCodes, const std::string& doctorLPK)
{
    Db db;
    db.newStatement("DELETE FROM favourite_code WHERE lpk=?");
    db.bind(1, doctorLPK);
    db.execute();

    for (auto& code : procedureCodes) {
        db.newStatement("INSERT INTO favourite_code (code, lpk) VALUES (?,?)");
        db.bind(1, code);
        db.bind(2, doctorLPK);

        if (!db.execute()) {
            return false;
        }
    }

    return true;
}

std::set<std::string> DbDoctor::getFavouriteProcedures(const std::string& doctorLPK)
{
    Db db;

    db.newStatement("SELECT code FROM favourite_code WHERE lpk=?");

    db.bind(1, doctorLPK);

    std::set<std::string> result;

    while (db.hasRows()) {
        result.insert(db.asString(0));
    }

    return result;
}

bool DbDoctor::isIncognito(const std::string& lpk)
{
    Db db;

    db.newStatement("SELECT incognito FROM doctor WHERE lpk=?");

    db.bind(1, lpk);

    while (db.hasRows()) {
        return db.asBool(0);
    }

    return false;
}

bool DbDoctor::setIncognito(bool incognito, const std::string& lpk)
{
    Db db("UPDATE doctor SET incognito=? WHERE lpk=?");
    db.bind(1, incognito);
    db.bind(2, lpk);

    return db.execute();
}
