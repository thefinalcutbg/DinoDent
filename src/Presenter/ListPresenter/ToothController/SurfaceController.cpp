#include "SurfaceController.h"

void ObturationController::addStatus(int statusIndex)
{
    bool& obturation = tooth->obturation;
    std::array<DentistMade, 6>& surfO = tooth->o_surf;

    if (!obturation)
        for (int i = 0; i < 6; i++) surfO[i].set(false); //resetting the surfaces, if obturation is non-present

    surfO[statusIndex].set(true);

    obturation = true;
    tooth->extraction.set(false);
    tooth->root.set(false);
    tooth->implant.set(false);
}

void ObturationController::removeStatus(int statusIndex)
{
    tooth->o_surf[statusIndex].set(false);

    for (int i = 0; i < tooth->o_surf.size(); i++)
        if (tooth->o_surf[i].exists()) return;

    tooth->obturation = false;
}

void ObturationController::removeStatus()
{
    tooth->obturation = false;
    for (int i = 0; i < 6; i++) tooth->o_surf[i].set(false);
}

void CariesController::addStatus(int statusIndex)
{
    bool& caries = tooth->caries;
    std::array<Pathology, 6>& surfC = tooth->c_surf;

    if (!caries)
        for (int i = 0; i < 6; i++) surfC[i].set(false); //resetting the surfaces, if obturation is non-present

    surfC[statusIndex].set(true);

    caries = true;
    tooth->extraction.set(false);
    tooth->root.set(false);
    tooth->implant.set(false);
}

void CariesController::removeStatus(int statusIndex)
{
    tooth->c_surf[statusIndex].set(false);

    for (int i = 0; i < tooth->c_surf.size(); i++)
        if (tooth->c_surf[i].exists()) return;

    tooth->caries = false;
}

void CariesController::removeStatus()
{
    tooth->caries = false;
    for (int i = 0; i < 6; i++) tooth->c_surf[i].set(false);
}


