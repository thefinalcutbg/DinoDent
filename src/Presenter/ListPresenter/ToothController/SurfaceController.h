#pragma once
#include "ToothController.h"
#include "array"


class ObturationController : public ToothController
{

public:
    void addStatus(int statusIndex);
    void removeStatus(int statusIndex);
    void removeStatus();
};

class CariesController : public ToothController
{
public:
    void addStatus(int statusIndex);
    void removeStatus(int statusIndex);
    void removeStatus();
};
