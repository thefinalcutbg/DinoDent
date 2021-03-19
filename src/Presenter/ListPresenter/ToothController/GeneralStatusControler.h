#pragma once
#include "ToothController.h"
#include "Model/Tooth/ToothUtils.h"

class GeneralStatusController : public ToothController
{

    ToothType tooth_type;
    ToothUtils utils;

public:
    GeneralStatusController();

    void setTooth(Tooth* tooth);

    void addStatus(int statusIndex);
    void removeStatus(int statusIndex);
    void removeStatus();
};
