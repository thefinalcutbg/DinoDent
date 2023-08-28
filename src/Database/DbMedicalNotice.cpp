#include "DbMedicalNotice.h"
#include <Database/Database.h>

std::vector<MedicalNotice> DbMedicalNotice::get(long long amblistRowid)
{
    return std::vector<MedicalNotice>();
}

void DbMedicalNotice::save(const std::vector<MedicalNotice> notice, long long amblistRowid, Db& db)
{
}

void DbMedicalNotice::save(const std::vector<MedicalNotice> notice, long long amblistRowid)
{
}
