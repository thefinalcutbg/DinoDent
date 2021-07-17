#include "AmbListValidator.h"
#include "../Presenter/TabPresenter/ListInstance.h"
#include "Procedure/MasterNZOK.h"
#include "Tooth/ToothUtils.h"

AmbListValidator::AmbListValidator()
{
    _error.reserve(100);
}

bool AmbListValidator::counter(const Procedure& p, const std::string& patientID)
{
    return false;
}

bool AmbListValidator::validateTypeToStatus(const Tooth& t, const Procedure& p)
{
    bool extracted = t.extraction.exists();
    bool implant = t.implant.exists();

    std::string toothNum = std::to_string(ToothUtils::getToothNumber(t.index, t.temporary.exists()));
    std::string code = std::to_string(p.code);

    switch (p.type)
    {
        case ProcedureType::obturation:
        {
            if (extracted || implant)
            {
                _error = "Зъб " + toothNum + " е вече екстрахиран и манипулация " + code + " не може да бъде извършена!";
                return false;
            }

            bool statusMissing
            {
                !t.caries.exists() &&
                !t.pulpitis.exists() &&
                !t.lesion.exists() &&
                !t.root.exists() &&
                !t.fracture.exists()
            };

            if (statusMissing)
            {
                _error = "За манипулация " + code + " на зъб " + toothNum + " не е въведен валиден статус. " +
                    "Валидните статуси включват кариес, обтурация, пулпит, периодонтит, корен или фрактура.";
                return false;
            }
            break;
        }

        case ProcedureType::endo:
        {
            if (extracted || implant)
            {
                _error = "Зъб " + toothNum + " е вече екстрахиран и манипулация " + code + " не може да бъде извършена!";
                return false;
            }

            bool statusMissing
            {
                !t.pulpitis.exists() &&
                !t.lesion.exists() &&
                !t.fracture.exists()
            };


            if (statusMissing)
            {
                _error = "За манипулация " + code + " на зъб " + toothNum + " не е въведен валиден статус. " +
                    "Валидните статуси включват пулпит, периодонтит или фрактура.";
                return false;
            }
            break;
        }

         case ProcedureType::extraction:
         {
            if (extracted)
            {
                _error = "Зъб " + toothNum + " е вече екстрахиран и манипулация " + code + " не може да бъде извършена!";
                return false;
            }

        bool statusMissing 
        {
            !t.pulpitis.exists() &&
            !t.periodontitis.exists() &&
            !t.fracture.exists() &&
            !t.hyperdontic.exists() &&
            !t.implant.exists() &&
            !t.temporary.exists() &&
            !t.lesion.exists() &&
            !t.root.exists() &&
            !t.mobility.exists() 
        };

        if (statusMissing)
        {
            _error = "За манипулация " + code + " на зъб " + toothNum + " не е въведен валиден статус. " +
                "Валидните статуси включват периодонтит, корен, фрактура, пулпит, пародонтит, подвижност, имплант, свръхброен или временен зъб.";
            return false;
        }
        break;

    }

        default: break;
    }

    return true;
}

bool AmbListValidator::validatePermaTemp(const Tooth& tooth, const Procedure& p)
{
    bool temp = tooth.temporary.exists();

    std::string toothNum = std::to_string(ToothUtils::getToothNumber(p.tooth, temp));

    if (MasterNZOK::instance().isTempOnly(p.code) && !temp)
    {
        _error = "Манипулация "+ std::to_string(p.code) + " на зъб " + toothNum + " е позволена само при временни зъби";
        return false;
    }
    
    if (MasterNZOK::instance().isPermaOnly(p.code) && temp)
    {
        _error = "Манипулация " + std::to_string(p.code) + " на зъб " + toothNum + " е позволена само при постоянни зъби";
        return false;
    }

    return true;
}


bool AmbListValidator::ambListIsValid(const ListInstance& list)
{
    auto& teeth = list.amb_list.teeth;
    auto& procedures = list.amb_list.procedures;


    auto totalProcedures = _db.totalNZOKProcedures(list.patient->id, list.amb_list.id, list.amb_list.date.year);

    for (auto& p : procedures)
    {
        if (!p.nzok) continue;

        //checking if the procedure is allowed depending on patient age
        if (MasterNZOK::instance().isMinorOnly(p.code) && list.patient->isAdult(p.date))
        {
            _error = "Манипулация " + std::to_string(p.code) + " е позволена само при лица под 18 годишна възраст!";
            return false;
        }

        totalProcedures[p.code]++;

        if (p.tooth != -1) //out of range guard
        {
            //checking temporary/permanent tooth requirement of the procedure
            if (!validatePermaTemp(teeth[p.tooth], p)) return false;

            //checking if the tooth has appliable status
            if (!validateTypeToStatus(teeth[p.tooth], p)) return false;
        }
    }

    //refactor l8r

    if (totalProcedures[101] > 1)
    {
        _error = "Позволен е максимум един преглед в годината! "
                 "В момента са открити: " + std::to_string(totalProcedures[101]);
        return false;
    }

    if (totalProcedures[301] + totalProcedures[509] > 3)
    {
        _error = "Надвишен лимит на манипулации по НЗОК!";
        return false;
    }

    _error = "";
    return true;
}

const std::string& AmbListValidator::getErrorMsg()
{
    return _error;
}
