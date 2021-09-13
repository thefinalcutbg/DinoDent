#pragma once
#include <vector>
#include "Model/Procedure/Procedure.h"

class ProcedureDialogPresenter;
class ProcedurePresenter;
class AbstractComboBox;
class IProcedureView
{
public:

	virtual void setProcedurePresenter(ProcedurePresenter* presenter) = 0;

};