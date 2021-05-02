#include "SurfaceController.h"

void ObturationController::addStatus(int statusIndex)
{
    
    tooth->obturation.set(true, statusIndex);
    tooth->extraction.set(false);
    tooth->root.set(false);
    tooth->implant.set(false);
}

void ObturationController::removeStatus(int statusIndex)
{
    tooth->obturation.set(false, statusIndex);
}

void ObturationController::removeStatus()
{
    tooth->obturation.reset();
}

void CariesController::addStatus(int statusIndex)
{
    tooth->caries.set(true, statusIndex);

    tooth->extraction.set(false);
    tooth->root.set(false);
    tooth->implant.set(false);
}

void CariesController::removeStatus(int statusIndex)
{
    tooth->caries.set(false, statusIndex);
}

void CariesController::removeStatus()
{
    tooth->caries.reset();
}


