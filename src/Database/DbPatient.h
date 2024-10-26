#pragma once

#include "Model/Patient.h"
#include <queue>
class Db;


namespace DbPatient
{
    long long insert(const Patient& patient);
    bool update(const Patient& patient);
    Patient get(std::string patientID, int type);
    Patient get(long long rowid);

    bool updateMedStatus(long long patientRowId, const MedicalStatuses& s);
    bool updateMedStatus(long long patientRowId, const MedicalStatuses& s, Db& db);

    MedicalStatuses getMedicalStatuses(long long patientRowId);
    MedicalStatuses getMedicalStatuses(long long patientRowId, Db& db);

    std::vector<Allergy> getAllergies(long long patientRowid);
    std::vector<Allergy> getAllergies(long long patientRowid, Db& db);
    bool updateAllergies(long long patientRowid, const std::vector<Allergy>& allergies);

    std::queue<Patient> getPatientList(const Date& visitAfter, const std::string& rzi, const std::string lpk);
    Date getLastVisit(long long patientRowid, const std::string& rzi, const std::string lpk);

    long long getPatientRowid(const std::string& firstName, const std::string& birth);

    TeethNotes getToothNotes(long long patientRowId);
};

