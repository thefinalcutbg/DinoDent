#pragma once
#include <vector>
#include <array>
#include "Model/Manipulation/Manipulation.h"


class CrownPresenter;
class ImplantPresenter;
class ObturationPresenter;

class IProcedureDialog
{

public:

    virtual void loadManipulationList(std::vector<ManipulationTemplate> manipulationList) = 0;

    virtual void close() = 0;
    virtual void showErrorMessage(const std::string& error) = 0;

    virtual void setView(ManipulationType t) = 0;
    virtual void setSelectionLabel(const std::vector<int>& selectedTeethNum) = 0;

    virtual ICommonFields* commonFields() = 0;
    virtual void setObturationPresenter(ObturationPresenter* presenter) = 0;
    virtual void setCrownPresenter(CrownPresenter* presenter) = 0;
    virtual void setImplantPresenter(ImplantPresenter* presenter) = 0;
};
