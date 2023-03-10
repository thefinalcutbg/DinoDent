#pragma once
#include "Model/Dental/Procedure.h"

#include "View/Interfaces/IImplantView.h"
#include "View/Interfaces/AbstractSurfaceSelector.h"
#include "View/Interfaces/ICrownView.h"
#include "View/Interfaces/IProcedureInput.h"
#include "View/Interfaces/AbstractRangeEdit.h"

class IProcedureEditDialog
{

public:

	virtual IProcedureInput* procedureInput() = 0;
	virtual void closeDialog() = 0;
};