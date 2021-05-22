#pragma once
#include <vector>
#include <array>
#include "Model/Manipulation/Manipulation.h"
#include "Presenter/ProcedureDialog/SubPresenters/ObturationPresenter.h"
#include "Presenter/ProcedureDialog/SubPresenters/CrownPresenter.h"

enum class FormView { DefaultView, surface, bridge, material };

class IProcedureDialog
{

public:

    virtual void loadManipulationList(std::vector<ManipulationTemplate> manipulationList) = 0;
    virtual void openProcedureDialog() = 0;
    virtual void close() = 0;
    virtual void showErrorMessage(const std::string& error) = 0;
    virtual void showErrorDialog(const std::string& error) = 0;
    virtual void setView(ManipulationType t) = 0;
    virtual void setSelectionLabel(const std::vector<int>& selectedTeethNum) = 0;

    virtual ICommonFields* commonFields() = 0;
    virtual void setObturationPresenter(ObturationPresenter* presenter) = 0;
    virtual void setCrownPresenter(CrownPresenter* presenter) = 0;
};
