#include "DbDoctor.h"
#include "Database.h"

std::optional<Doctor> DbDoctor::getDoctor(const std::string& lpk, const std::string& pass)
{
    std::optional<Doctor> result;

    std::string query =
        "SELECT fname, mname, lname, phone, several_rhif, his_specialty FROM doctor "
        "WHERE lpk = '" + lpk + "' "
        "AND pass = '" + pass + "' ";

    for (Db db(query); db.hasRows();)
    {
        Doctor doctor;

        doctor.LPK = lpk;
        doctor.fname = db.asString(0);
        doctor.mname = db.asString(1);
        doctor.lname = db.asString(2);
        doctor.phone = db.asString(3);
        doctor.severalRHIF = db.asInt(4);
        doctor.pass = pass;
        doctor.hisSpecialty = db.asInt(5);

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

bool DbDoctor::updateDoctor(const Doctor& doctor, std::string& currentLPK)
{
   Db db;

   db.newStatement(
       "UPDATE doctor SET "
       "lpk=?,"
       "fname=?,"
       "mname=?,"
       "lname=?,"
       "pass=?,"
       "phone=?,"
       "several_rhif=?,"
       "his_specialty=? "
       "WHERE lpk=?"
   );

   db.bind(1, doctor.LPK);
   db.bind(2, doctor.fname);
   db.bind(3, doctor.mname);
   db.bind(4, doctor.lname);
   db.bind(5, doctor.pass);
   db.bind(6, doctor.phone);
   db.bind(7, doctor.severalRHIF);
   db.bind(8, doctor.hisSpecialty.getIdx());
   db.bind(9, currentLPK);

   return db.execute();
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
