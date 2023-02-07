#include "DbDoctor.h"
#include "Database.h"

std::optional<Doctor> DbDoctor::getDoctor(const std::string& lpk, const std::string& pass)
{
    std::optional<Doctor> result;

    std::string query =
        "SELECT fname, mname, lname, egn, phone, several_rhif, his_specialty FROM doctor "
        "WHERE lpk = '" + lpk + "' "
        "AND pass = '" + pass + "' ";

    for (Db db(query); db.hasRows();)
    {
        Doctor doctor;

        doctor.LPK = lpk;
        doctor.fname = db.asString(0);
        doctor.mname = db.asString(1);
        doctor.lname = db.asString(2);
        doctor.egn = db.asString(3);
        doctor.phone = db.asString(4);
        doctor.severalRHIF = db.asInt(5);
        doctor.pass = pass;
        doctor.hisSpecialty = db.asInt(6);

        result = doctor;
    }

    return result;
}

std::optional<Doctor> DbDoctor::getDoctor(const std::string& lpk)
{
    std::optional<Doctor> result;

    std::string query =
        "SELECT fname, mname, lname, egn, phone, several_rhif, pass, his_specialty FROM doctor "
        "WHERE lpk = '" + lpk + "'";

    for (Db db(query); db.hasRows();)
    {
        Doctor doctor;

        doctor.LPK = lpk;
        doctor.fname = db.asString(0);
        doctor.mname = db.asString(1);
        doctor.lname = db.asString(2);
        doctor.egn = db.asString(3);
        doctor.phone = db.asString(4);
        doctor.severalRHIF = db.asInt(5);
        doctor.pass = db.asString(6);
        doctor.hisSpecialty = db.asInt(7);


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

void DbDoctor::setAutoLogin(const std::string& lpk, bool remember)
{
    std::string query =
        "UPDATE doctor SET auto_login="
        "CASE WHEN lpk = ? THEN ? ELSE 0 END";


    Db db(query);
    db.bind(1, lpk);
    db.bind(2, remember);
    db.execute();

}

void DbDoctor::updateDoctor(const Doctor& doctor, std::string& currentLPK)
{
    Db::crudQuery(

        "UPDATE doctor SET "
        "lpk = '" + doctor.LPK + "', "
        "fname = '" + doctor.fname + "', "
        "mname = '" + doctor.mname + "', "
        "lname = '" + doctor.lname + "', "
        "pass = '" + doctor.pass + "', "
        "egn = '" + doctor.egn + "', "
        "phone = '" + doctor.phone + "', "
        "several_rhif = " + std::to_string(doctor.severalRHIF) + ", "
        "his_specialty=" + std::to_string(doctor.hisSpecialty.getIdx()) + " "
        "WHERE lpk = '" + currentLPK + "' "

    );
}

void DbDoctor::insertDoctor(const Doctor& doctor)
{
    Db db(
        "INSERT INTO doctor (lpk, fname, mname, lname, pass, egn, phone, several_rhif, auto_login, his_specialty ) "
        "VALUES(?,?,?,?,?,?,?,?,?,?)"
    );

    db.bind(1, doctor.LPK);
    db.bind(2, doctor.fname);
    db.bind(3, doctor.mname);
    db.bind(4, doctor.lname);
    db.bind(5, doctor.pass);
    db.bind(6, doctor.egn);
    db.bind(7, doctor.phone);
    db.bind(8, doctor.severalRHIF);
    db.bind(9, false);
    db.bind(10, doctor.hisSpecialty.getIdx());

    db.execute();
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
