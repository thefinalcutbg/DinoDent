#pragma once

#include "Model/Patient.h"
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
    TeethNotes getPresentNotes(long long patientRowId);
};

