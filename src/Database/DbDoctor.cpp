#include "DbDoctor.h"
#include "Database.h"

std::optional<Doctor> DbDoctor::getDoctor(const std::string& lpk, const std::string& pass)
{
    std::optional<Doctor> result;

    std::string query =
        "SELECT fname, mname, lname, egn, spec, several_rhif FROM doctor "
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
        doctor.specialty = db.asInt(4);
        doctor.severalRHIF = db.asInt(5);

        doctor.pass = pass;

        result = doctor;
    }

    return result;
}

std::optional<Doctor> DbDoctor::getDoctor(const std::string& lpk)
{
    std::optional<Doctor> result;

    std::string query =
        "SELECT fname, mname, lname, egn, spec, several_rhif, pass FROM doctor "
        "WHERE lpk = '" + lpk + "'";

    for (Db db(query); db.hasRows();)
    {
        Doctor doctor;

        doctor.LPK = lpk;
        doctor.fname = db.asString(0);
        doctor.mname = db.asString(1);
        doctor.lname = db.asString(2);
        doctor.egn = db.asString(3);
        doctor.specialty = db.asInt(4);
        doctor.severalRHIF = db.asInt(5);
        doctor.pass = db.asString(6);

        result = doctor;
    }

    return result;

}

void DbDoctor::updateDoctor(const Doctor& doctor, std::string& currentLPK)
{
    Db::crudQuery(

        "UPDATE doctor SET "
        "lpk = '" + doctor.LPK + "', "
        "spec = " + std::to_string(doctor.specialty) + ", "
        "fname = '" + doctor.fname + "', "
        "mname = '" + doctor.mname + "', "
        "lname = '" + doctor.lname + "', "
        "pass = '" + doctor.pass + "', "
        "egn = '" + doctor.egn + "', "
        "several_rhif = " + std::to_string(doctor.severalRHIF) + " "
        "WHERE lpk = '" + currentLPK + "' "

    );
}

void DbDoctor::insertDoctor(const Doctor& doctor)
{
    Db::crudQuery(

        "INSERT INTO doctor (lpk, spec, fname, mname, lname, pass, egn, several_rhif) "
        "VALUES('"
        + doctor.LPK + "', "
        + std::to_string(doctor.specialty) + ", "
        "'" + doctor.fname + "', "
        "'" + doctor.mname + "', "
        "'" + doctor.lname + "', "
        "'" + doctor.pass + "', "
        "'" + doctor.egn + "', "
        + std::to_string(doctor.severalRHIF) + ")"

    );
}

bool DbDoctor::getAdminPremission(const std::string& lpk, const std::string& rzi)
{
    bool admin{ false };

    std::string query = "SELECT admin "
        "FROM practice_doctor "
        "WHERE doctor_lpk ='" + lpk + "' "
        "AND practice_rzi = '" + rzi + "'";

    for(Db db(query); db.hasRows();){
        return db.asInt(0);
    }

    return false;
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