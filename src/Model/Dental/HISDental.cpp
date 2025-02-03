#include "HisDental.h"
#include <map>
#include <functional>
#include <algorithm>

struct Ranges {

    std::vector<int> splints;
    std::vector<int> bridges;
    std::vector<int> pontics;

    void removeIdx(int index) {

        auto exists = [=](int other) {
            return index == other;
            };

        std::vector<int>::iterator it;

        it = std::remove_if(splints.begin(), splints.end(), exists);
        it = std::remove_if(bridges.begin(), bridges.end(), exists);
        it = std::remove_if(pontics.begin(), pontics.end(), exists);

    }

    void sortRanges()
    {
        std::sort(splints.begin(), splints.end());
        std::sort(bridges.begin(), bridges.end());
        std::sort(pontics.begin(), pontics.end());
    }

};

void deserializeStatus(Tooth& tooth, Ranges& ranges, const std::string& code);

ToothContainer HISToothContainer::getToothContainer() const
{
    ToothContainer result;

    Ranges ranges;

    for (auto& [toothIdx, conditions] : *this)
    {
        result[toothIdx.index].setStatus(Dental::HasSupernumeral, toothIdx.supernumeral);

        auto& tooth = result.at(toothIdx);

        tooth.setStatus(Dental::Temporary, toothIdx.temp);

        for (auto& code : conditions)
        {
            deserializeStatus(tooth, ranges, code);
        }
    }

    ranges.sortRanges();

    result.setStatus(ranges.splints, Dental::StatusType::General, Dental::Splint, true);
    result.setStatus(ranges.bridges, Dental::StatusType::General, Dental::Bridge, true);

    //in case extraction is not set on pontics:
    for (auto idx : ranges.pontics) if (result[idx].canHaveACrown()) result[idx].setStatus(Dental::Missing, true);

    return result;
}

std::string HISToothContainer::getToothString() const
{
    if (size() == 1) return this->at(0).idx.getNhifNumenclature();

    return "";
}

std::vector<const Tooth*> HISToothContainer::applyProcedure(ToothContainer& teeth) const
{
    teeth.copyFromHis(getToothContainer());

    std::vector<const Tooth*> result;

    for (auto& t : *this) {
        result.push_back(&teeth.at(t.idx));
    }

    return result;
}

std::vector<int> HISToothContainer::getArrayIndexes() const
{
    std::vector<int> result;

    for (auto& t : *this) {
        result.push_back(t.idx.index);
    }

    return result;
}

void deserializeStatus(Tooth& tooth, Ranges& r, const std::string& code)
{

    static std::map<std::string, std::function<void(Tooth& tooth, Ranges& r)>> lambdaMap
    {
        { "E",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Missing, true); } },
        { "T",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Calculus, true); } },
        { "K",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Crown, true); } },
        { "B",	[](Tooth& tooth, Ranges& r) mutable { r.bridges.push_back(tooth.index()); r.pontics.push_back(tooth.index()); } },
        { "Kb",	[](Tooth& tooth, Ranges& r) mutable { r.bridges.push_back(tooth.index()); } },
        { "O",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Restoration, true); } },
        { "C",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Caries, true); } },
        { "Oo",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Restoration, Dental::Surface::Occlusal); } },
        { "Om",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Restoration, Dental::Surface::Medial);  } },
        { "Od",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Restoration, Dental::Surface::Distal);  } },
        { "Ob",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Restoration, Dental::Surface::Buccal); } },
        { "Ol",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Restoration, Dental::Surface::Lingual);  } },
        { "Oc",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Restoration, Dental::Surface::Cervical);  } },
        { "Co",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Caries, Dental::Surface::Occlusal); } },
        { "Cm",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Caries, Dental::Surface::Medial);  } },
        { "Cd",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Caries, Dental::Surface::Distal);  } },
        { "Cb",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Caries, Dental::Surface::Buccal); } },
        { "Cl",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Caries, Dental::Surface::Lingual);  } },
        { "Cc",	[](Tooth& tooth, Ranges&) mutable { tooth.setSurface(Dental::Caries, Dental::Surface::Cervical);  } },
        { "M1",	[](Tooth& tooth, Ranges&) mutable { tooth.setMobility(Dental::I, true); } },
        { "M2",	[](Tooth& tooth, Ranges&) mutable { tooth.setMobility(Dental::II, true); } },
        { "M3",	[](Tooth& tooth, Ranges&) mutable { tooth.setMobility(Dental::III, true); } },
        { "X",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Denture, true); } },
        { "R",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Root, true); } },
        { "Rc",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::RootCanal, true); } },
        { "Rp",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Post, true); } },
        { "H",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Healthy, true); } },
        { "I",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Implant, true); } },
        { "Re",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Impacted, true); } },
        { "G",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::ApicalLesion, true); } },
        { "P",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Pulpitis, true); } },
        { "F",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Fracture, true); } },
        { "Pa",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::Periodontitis, true); } },
        { "D",	[](Tooth& tooth, Ranges&) mutable { tooth.setStatus(Dental::HasSupernumeral, true); } },
        { "S",	[](Tooth& tooth, Ranges& r) mutable { r.splints.push_back(tooth.index()); } }
    };

    if (lambdaMap.count(code)) lambdaMap[code](tooth, r);
}
