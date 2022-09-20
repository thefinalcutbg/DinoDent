#include "DbDoctor.h"
#include "Database.h"

std::optional<Doctor> DbDoctor::getDoctor(const std::string& lpk, const std::string& pass)
{
    std::optional<Doctor> result;

    std::string query =
        "SELECT fname, mname, lname, egn, phone, several_rhif FROM doctor "
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

        result = doctor;
    }

    return result;
}

std::optional<Doctor> DbDoctor::getDoctor(const std::string& lpk)
{
    std::optional<Doctor> result;

    std::string query =
        "SELECT fname, mname, lname, egn, phone, several_rhif, pass FROM doctor "
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

        result = doctor;
    }

    return result;

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
        "several_rhif = " + std::to_string(doctor.severalRHIF) + " "
        "WHERE lpk = '" + currentLPK + "' "

    );
}

void DbDoctor::insertDoctor(const Doctor& doctor)
{
    Db::crudQuery(

        "INSERT INTO doctor (lpk, fname, mname, lname, pass, egn, phone, several_rhif) "
        "VALUES('"
        + doctor.LPK + "', "
        "'" + doctor.fname + "', "
        "'" + doctor.mname + "', "
        "'" + doctor.lname + "', "
        "'" + doctor.pass + "', "
        "'" + doctor.egn + "', "
        "'" + doctor.phone + "', "
        + std::to_string(doctor.severalRHIF) + ")"

    );
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
