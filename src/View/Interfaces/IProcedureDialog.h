#pragma once
#include <vector>
#include <array>
#include "Model/Dental/Procedure.h"


class CrownPresenter;
class ImplantPresenter;
class ObturationPresenter;
class FiberSplintPresenter;
class ICommonFields;

class IProcedureDialog
{

public:

    virtual void setProcedureTemplates(std::vector<ProcedureTemplate> manipulationList) = 0;

    virtual void close() = 0;
    virtual void showErrorMessage(const std::string& error) = 0;

    virtual void setView(ProcedureTemplateType t) = 0;
    virtual void setSelectionLabel(const std::vector<int>& selectedTeethNum) = 0;

    virtual ICommonFields* commonFields() = 0;
    virtual void setObturationPresenter(ObturationPresenter* presenter) = 0;
    virtual void setCrownPresenter(CrownPresenter* presenter) = 0;
    virtual void setImplantPresenter(ImplantPresenter* presenter) = 0;
    virtual void setFiberSplintPresenter(FiberSplintPresenter* presenter) = 0;
};