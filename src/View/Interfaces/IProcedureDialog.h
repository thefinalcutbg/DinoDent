#pragma once
#include <vector>
#include <array>
#include "Model/Dental/Procedure.h"

class AbstractSurfaceSelector;
class AbstractRangeEdit;
class IProcedureInput;
class ICrownView;

class IProcedureDialog
{

public:

    virtual void setProcedureTemplates(std::vector<ProcedureCode> procedureList) = 0;

    virtual void close() = 0;

    virtual void setSelectionLabel(const std::vector<int>& selectedTeethNum) = 0;

    virtual IProcedureInput* procedureInput() = 0;


};
