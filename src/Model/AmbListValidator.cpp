#include <unordered_set>

#include "AmbListValidator.h"
#include "../Presenter/TabPresenter/ListInstance.h"
#include "Procedure/MasterNZOK.h"
#include "Tooth/ToothUtils.h"
#include "Model/Procedure/PackageCounter.h"


AmbListValidator::AmbListValidator(const ListInstance& list)
    :
    ambList(list.amb_list), patient(*list.patient.get())
{
    _error.reserve(100);
}


bool AmbListValidator::ambListIsValid()
{
    auto& teeth = ambList.teeth;
    auto& procedures = ambList.procedures;

    auto totalProcedures = _db.totalNZOKProcedures(patient.id, ambList.id, ambList.date.year);

    PackageCounter packageCounter(MasterNZOK::instance().getPackages(ambList.date));

    for (auto& t : totalProcedures)
    {
        for (int i = 0; i < t.second; i++) packageCounter.insertCode(t.first);
    }

    for (auto& p : procedures)
    {
        if (p.date < ambList.date || p.date > ambList.date.getMaxDateOfMonth())
        {
            _error = "Датата на манипулация " + std::to_string(p.code) + " е невалидна по отношение на датата на амбулаторния лист";
            return false;
        }

        if (!p.nzok) continue;

        //checking if the procedure is allowed depending on patient age
        if (MasterNZOK::instance().isMinorOnly(p.code) && patient.isAdult(p.date))
        {
            _error = "Манипулация " + std::to_string(p.code) + " е позволена само при лица под 18 годишна възраст!";
            return false;
        }

        if (!noDuplicates()) return false;

        totalProcedures[p.code]++; 

        packageCounter.insertCode(p.code);

        if (!packageCounter.validate(patient.isAdult(p.date), 0))
        {
            _error = "Надвишен лимит по НЗОК за код " + std::to_string(p.code) + "!";
            return false;
        };


        if (p.tooth != -1) //out of range guard
        {
            //checking temporary/permanent tooth requirement of the procedure
            if (!validatePermaTemp(teeth[p.tooth], p)) return false;

            //checking if the tooth has appliable status
            if (!validateTypeToStatus(teeth[p.tooth], p)) return false;

            //checking if the tooth has extraction recorded
            if (isExtracted(teeth[p.tooth]))
            {
                _error = "За зъб " + ToothUtils::getNomenclature(teeth[p.tooth]) + " съществуват предишни данни за екстракция";
                return false;
            }

            //checking if the manipulation is made in the last year
            if (!madeAtLeastYearAgo(p.tooth, p)) return false;

        }



    }


    _error = "";
    return true;
}


struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

bool AmbListValidator::noDuplicates()
{
    typedef int Tooth, Code;

    std::unordered_set<std::pair<Tooth, Code>, pair_hash> tooth_set;

    for (auto& p : ambList.procedures)
    {
        if (!p.nzok) continue;

        auto pair = std::make_pair(p.tooth, p.code);

        if (tooth_set.count(pair))
        {
            p.tooth != -1 ?
            _error = "За зъб " + ToothUtils::getNomenclature(ambList.teeth[p.tooth]) +
                " манипулация с код " + std::to_string(p.code) + " е добавена повече от веднъж"
            :
            _error = "Направили сте 2 еднакви манипулации с код " + std::to_string(p.code);

            return false;
        }

        tooth_set.insert(pair);
    }

    return true;
}

bool AmbListValidator::validateTypeToStatus(const Tooth& t, const Procedure& p)
{
    bool extracted = t.extraction.exists();
    bool implant = t.implant.exists();

    std::string toothNum = ToothUtils::getNomenclature(t);
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
                _error = "За манипулация " + code + " на зъб " + toothNum + " не е въведен валиден статус. "
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
            _error = "За манипулация " + code + " на зъб " + toothNum + " не е въведен валиден статус. "
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

    if (MasterNZOK::instance().isTempOnly(p.code) && !temp)
    {
        _error = "Манипулация "+ std::to_string(p.code) + " на зъб " + ToothUtils::getNomenclature(p.tooth, temp) + " е позволена само при временни зъби";
        return false;
    }
    
    if (MasterNZOK::instance().isPermaOnly(p.code) && temp)
    {
        _error = "Манипулация " + std::to_string(p.code) + " на зъб " + ToothUtils::getNomenclature(p.tooth, temp) + " е позволена само при постоянни зъби";
        return false;
    }

    return true;
}

bool AmbListValidator::madeAtLeastYearAgo(int tooth, const Procedure& p)
{
    bool invalid = _db.procedureExists(p.tooth, p.code, patient.id, p.date, 1, ambList.id);
    
    if(invalid)
    {
        _error = "Манипулацията " + std::to_string(p.code) +
            " на зъб " + ToothUtils::getNomenclature(ambList.teeth[p.tooth]) +
            " е вече направена в последната година";
        return false;
    };

    return true;
}

bool AmbListValidator::isExtracted(const Tooth& tooth)
{
    return  tooth.temporary.exists() ?

        _db.procedureExists(tooth.index, 508, patient.id, ambList.id)
        :
        _db.procedureExists(tooth.index, 509, patient.id, ambList.id); //what about 510 ?

}


const std::string& AmbListValidator::getErrorMsg()
{
    return _error;
}
