#pragma once
#include <vector>
#include <array>
#include "Model/Dental/ProcedureListElement.h"

class AbstractSurfaceSelector;
class AbstractRangeEdit;
class IProcedureInput;
class ICrownView;

class IProcedureDialog
{

public:

    virtual void setProcedureSections(const std::vector<std::string>& sectionNames, int defaultIdx) = 0;

    virtual void setProcedureTemplates(std::vector<ProcedureListElement> procedureList) = 0;

    virtual void close() = 0;

    virtual void setSelectionLabel(const std::vector<int>& selectedTeethNum) = 0;

    virtual IProcedureInput* procedureInput() = 0;


};
