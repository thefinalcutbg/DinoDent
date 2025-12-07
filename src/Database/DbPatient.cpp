#include "DbPatient.h"
#include "Database.h"
#include "Database/DbNotes.h"

long long DbPatient::insert(const Patient& patient)
{
    Db db(
        "INSERT INTO patient "
        "(type, id, birth, sex, fname, mname, lname, "
        "ekatte, address, hirbno, phone, country, institution_num, ehic_num, date_valid, foreign_city, color) "
        "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
    );

    db.bind(1, patient.type);
    db.bind(2, patient.id);
    db.bind(3, patient.birth.to8601());
    db.bind(4, patient.sex);
    db.bind(5, patient.FirstName);
    db.bind(6, patient.MiddleName);
    db.bind(7, patient.LastName);
    db.bind(8, patient.city.getIdxAsInt());
    db.bind(9, patient.address);
    db.bind(10, patient.HIRBNo);
    db.bind(11, patient.phone);

    if (patient.foreigner)
    {
        db.bind(12, patient.foreigner->country.getCode());
        db.bind(13, patient.foreigner->institution);
        db.bind(14, patient.foreigner->ehic);
        db.bind(15, patient.foreigner->date_valid.to8601());
        db.bind(16, patient.foreigner->city);
    }

    db.bind(17, patient.colorNameRgb);

    if (db.execute()) return db.lastInsertedRowID();

    return 0;

}

bool DbPatient::update(const Patient& patient)
{
    Db db(

        "UPDATE patient SET "
        "type=?,"
        "birth=?,"
        "sex=?,"
        "fname=?,"
        "mname=?,"
        "lname=?,"
        "ekatte=?,"
        "address=?,"
        "hirbno=?,"
        "phone=?,"
        "country=?,"
        "institution_num=?,"
        "ehic_num=?,"
        "date_valid=?, "
        "foreign_city=?, "
        "color=? "
        "WHERE rowid=?"
    );

    db.bind(1, patient.type);
    db.bind(2, patient.birth.to8601());
    db.bind(3, patient.sex);
    db.bind(4, patient.FirstName);
    db.bind(5, patient.MiddleName);
    db.bind(6, patient.LastName);
    db.bind(7, patient.city.getIdxAsInt());
    db.bind(8, patient.address);
    db.bind(9, patient.HIRBNo);
    db.bind(10, patient.phone);
    

    if (patient.foreigner)
    {
        db.bind(11, patient.foreigner->country.getCode());
        db.bind(12, patient.foreigner->institution);
        db.bind(13, patient.foreigner->ehic);
        db.bind(14, patient.foreigner->date_valid.to8601());
        db.bind(15, patient.foreigner->city);
    }
    
    db.bind(16, patient.colorNameRgb);

    db.bind(17, patient.rowid);

    return db.execute();

}

Patient DbPatient::get(const std::string& patientID, int type)
{
    std::string query = "SELECT rowid, type, id, birth, sex, fname, mname, lname, ekatte, address, hirbno, phone, country, foreign_city, institution_num, ehic_num, date_valid, color "
        "FROM patient WHERE id = ? "
        "AND type = ?";

    Db db(query);

    db.bind(1, patientID);
    db.bind(2, type);

    Patient patient;

    
    patient.type = static_cast<Patient::Type>(type);
    patient.id = patientID;

    if (patient.type == Patient::Type::EU) {
        patient.foreigner.emplace();
    }

    while (db.hasRows())
    {
        patient.rowid = db.asRowId(0),
        patient.birth = Date(db.asString(3));
        patient.sex = Patient::Sex(db.asInt(4));
        patient.FirstName = db.asString(5);
        patient.MiddleName = db.asString(6);
        patient.LastName = db.asString(7);
        patient.city = Ekatte(db.asInt(8));
        patient.address = db.asString(9);
        patient.HIRBNo = db.asString(10);
        patient.phone = db.asString(11);

        if (patient.type == Patient::EU)
        {
            patient.foreigner = Foreigner{
                .country = Country{db.asString(12)},
                .city = db.asString(13),
                .institution = db.asString(14),
                .ehic = db.asString(15),
                .date_valid = db.asString(16)
            };
        }

        patient.colorNameRgb = db.asString(17);
    }

    //ensured birth and sex are valid
    if (patient.type == Patient::EGN) {
        patient.birth = Date::getBirthdateFromEgn(patient.id);
        patient.sex = Patient::getSexFromEgn(patient.id);
    }

    patient.teethNotes = getToothNotes(patient.rowid);
    patient.patientNotes = DbNotes::getNote(patient.rowid, -1);
    patient.medStats = getMedicalStatuses(patient.rowid, db);
    patient.allergies = getAllergies(patient.rowid, db);
    return patient;
}

Patient DbPatient::get(long long rowid)
{
    Db db("SELECT rowid, type, id, birth, sex, fname, mname, lname, ekatte, address, hirbno, phone, country, foreign_city, institution_num, ehic_num, date_valid, color "
        "FROM patient WHERE rowid = " + std::to_string(rowid)
    );

    Patient patient;

    while (db.hasRows())
    {
        patient.rowid = db.asRowId(0),
        patient.type = Patient::Type(db.asInt(1));
        patient.id = db.asString(2);
        patient.birth = Date(db.asString(3));
        patient.sex = Patient::Sex(db.asInt(4));
        patient.FirstName = db.asString(5);
        patient.MiddleName = db.asString(6);
        patient.LastName = db.asString(7);
        patient.city = db.asInt(8);
        patient.address = db.asString(9);
        patient.HIRBNo = db.asString(10);
        patient.phone = db.asString(11);

        if (patient.type == Patient::EU)
        {
            patient.foreigner = Foreigner{
                .country = Country{db.asString(12)},
                .city = db.asString(13),
                .institution = db.asString(14),
                .ehic = db.asString(15),
                .date_valid = db.asString(16)
            };
        }

        patient.colorNameRgb = db.asString(17);
    }

    patient.teethNotes = getToothNotes(patient.rowid);
    patient.patientNotes = DbNotes::getNote(patient.rowid, -1);
    patient.medStats = getMedicalStatuses(rowid, db);
    patient.allergies = getAllergies(patient.rowid, db);

    return patient;
}

enum MedStatusType { CurrentDiseases=1, PastDiseases=2 };

bool DbPatient::updateMedStatus(long long patientRowId, const MedicalStatuses& s)
{
    Db db;

    return updateMedStatus(patientRowId, s, db);
}

bool DbPatient::updateMedStatus(long long patientRowId, const MedicalStatuses& s, Db& db)
{
    db.newStatement("DELETE FROM medical_status WHERE patient_rowid=?");

    db.bind(1, patientRowId);

    db.execute();

    auto lambda = [&](const std::vector<MedicalStatus>& list, MedStatusType t) {

        for (auto& status : list)
        {

            db.newStatement("INSERT INTO medical_status (patient_rowid, icd, type, nrn) VALUES (?,?,?,?)");

            db.bind(1, patientRowId);
            db.bind(2, status.diagnosis.code());
            db.bind(3, t);
            db.bind(4, status.nrn);

            db.execute();
        }
        };

    // lambda(s.allergies, Allergy);
    lambda(s.condition, CurrentDiseases);
    lambda(s.history, PastDiseases);

    return true;
}

MedicalStatuses DbPatient::getMedicalStatuses(long long patientRowId)
{
    Db db;

    return getMedicalStatuses(patientRowId, db);
}

MedicalStatuses DbPatient::getMedicalStatuses(long long patientRowId, Db& db)
{
    MedicalStatuses result;

    db.newStatement("SELECT type, icd, nrn FROM medical_status WHERE patient_rowid=?");
    db.bind(1, patientRowId);

    while (db.hasRows())
    {
        std::vector<MedicalStatus>* stat;

        switch (db.asInt(0))
        {
            //case Allergy: stat = &result.allergies; break;
        case CurrentDiseases: stat = &result.condition; break;
        case PastDiseases: stat = &result.history; break;
        default: continue;
        }

        stat->push_back(MedicalStatus(db.asString(1), db.asString(2)));
    }

    return result;
}

bool DbPatient::updateAllergies(long long patientRowid, const std::vector<Allergy>& allergies)
{
    Db db;

    db.newStatement("DELETE FROM allergy WHERE patient_rowid=?");

    db.bind(1, patientRowid);

    db.execute();

    for (auto& a : allergies)
    {

        db.newStatement(
            "INSERT INTO allergy "
            "(patient_rowid, lrn, nrn, description, type, category, criticality, clinical_status, verification_status, last_occurrence, edited) "
            "VALUES (?,?,?,?,?,?,?,?,?,?,?)"
        );

        db.bind(1, patientRowid);
        db.bind(2, a.lrn);
        db.bind(3, a.nrn);
        db.bind(4, a.description);
        db.bind(5, a.type);
        db.bind(6, a.category);
        db.bind(7, a.criticality);
        db.bind(8, a.clinicalStatus);
        db.bind(9, a.verificationStatus);
        db.bind(10, a.lastOccurrence ? a.lastOccurrence->to8601() : "");
        db.bind(11, a.edited);

        if (!db.execute()) return false;
    }

    return true;
}

std::queue <Patient> DbPatient::getPatientList(const Date& visitAfter, const std::string& rzi, const std::string lpk)
{
    std::string query =
        "SELECT patient.rowid, patient.id, patient.type, patient.fname, patient.lname, patient.phone, patient.birth, patient.color "
        "FROM patient LEFT JOIN amblist ON patient.rowid = amblist.patient_rowid "
        "WHERE date(amblist.date)>=? "
        "AND amblist.rzi = ? "
        "AND amblist.lpk = ? "
        "AND patient.type < 3 "
        "GROUP BY patient.rowid "
        "ORDER BY patient.rowid ASC";

    Db db(query);

    db.bind(1, visitAfter.to8601());
    db.bind(2, rzi);
    db.bind(3, lpk);
    
    std::queue<Patient> result;

    while (db.hasRows()) {
        Patient p;
        p.rowid = db.asRowId(0);
        p.id = db.asString(1);
        p.type = static_cast<Patient::Type>(db.asInt(2));
        p.FirstName = db.asString(3);
        p.LastName = db.asString(4);
        p.phone = db.asString(5);
        p.birth = db.asString(6);
        p.colorNameRgb = db.asString(7);
        result.push(p);
    }

    return result;
}

Date DbPatient::getLastVisit(long long patientRowid, const std::string& rzi, const std::string lpk)
{
    std::string query = R"(
        SELECT procedure.date FROM procedure 
        LEFT JOIN amblist ON amblist.rowid = procedure.amblist_rowid 
        LEFT JOIN patient ON amblist.patient_rowid = patient.rowid
        WHERE patient.rowid = ?
        AND amblist.rzi = ?
        AND amblist.lpk = ?
        ORDER BY procedure.date DESC
        LIMIT 1
    )";

    Db db(query);

    db.bind(1, patientRowid);
    db.bind(2, rzi);
    db.bind(3, lpk);

    while (db.hasRows()) {
        return db.asString(0);
    }

    return Date();

}

long long DbPatient::getPatientRowid(const std::string& firstName, const std::string& birth)
{
    Db db;

    db.newStatement("SELECT rowid FROM patient WHERE fname=? AND birth=?");

    db.bind(1, firstName);
    db.bind(2, birth);

    while(db.hasRows()){
        return db.asRowId(0);
    }

    return 0;
}

long long DbPatient::getPatientRowid(const std::string& id, int type)
{
    Db db;

    db.newStatement("SELECT rowid "
        "FROM patient WHERE id = ? AND type = ?"
    );

    db.bind(1, id);
    db.bind(2, type);

    while (db.hasRows()) {
        return db.asRowId(0);
    }

    return 0;
}

std::vector<DbPatient::PatientRecord> DbPatient::getPatientList()
{
    std::vector<PatientRecord> result;

    Db db("SELECT fname, lname, phone, birth, color FROM patient");

    while (db.hasRows())
    {
        PatientRecord r;
        r.fname = db.asString(0);
        r.summary = r.fname + " " + db.asString(1);
        
        auto phone = db.asString(2);

        if (phone.size()) {
            r.summary += " ";
            r.summary += phone;
			r.phone = phone;
        }

        r.birth = db.asString(3);     

        r.color = db.asString(4);

        result.push_back(r);
    }

    return result;
}

std::vector<Allergy> DbPatient::getAllergies(long long patientRowid, Db& db)
{
    db.newStatement(
        "SELECT lrn, nrn, description, type, category, criticality, clinical_status, verification_status, last_occurrence, edited FROM allergy "
        "WHERE patient_rowid=?"
    );

    db.bind(1, patientRowid);

    std::vector<Allergy> result;

    while (db.hasRows())
    {
        result.emplace_back();
        auto& a = result.back();

        a.lrn = db.asString(0);
        a.nrn = db.asString(1);
        a.description = db.asString(2);
        a.type = static_cast<Allergy::Type>(db.asInt(3));
        a.category = static_cast<Allergy::Category>(db.asInt(4));
        a.criticality = static_cast<Allergy::Criticality>(db.asInt(5));
        a.clinicalStatus = static_cast<Allergy::ClinicalStatus>(db.asInt(6));
        a.verificationStatus = static_cast<Allergy::VerificationStatus>(db.asInt(7));

        auto dateStr = db.asString(8);

        if (dateStr.size()) {
            a.lastOccurrence = Date{ dateStr };
        }

        a.edited = db.asBool(9);
    }

    return result;
}


std::vector<Allergy> DbPatient::getAllergies(long long patientRowid)
{
    Db db;

    return getAllergies(patientRowid, db);
}


TeethNotes DbPatient::getToothNotes(long long patientRowId)
{
    Db db("SELECT tooth, text FROM note WHERE patient_rowid=?");

    db.bind(1, patientRowId);

    TeethNotes notes{};

    while (db.hasRows())
    {
        auto index = db.asInt(0);
        //teeth notes range from 0 to 31
        if (index < 0 || index >=notes.size()) continue;

        notes[index] = db.asString(1);
    }

    return notes;
}