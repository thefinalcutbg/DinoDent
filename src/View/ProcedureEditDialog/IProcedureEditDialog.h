#pragma once
#include "Model/Procedure/Procedure.h"

#include "View/uiComponents/qt_derived/Widgets/IImplantView.h"
#include "View/ProcedureDialog/ProcedureFields/IObturationView.h"
#include "View/ProcedureDialog/ProcedureFields/ICrownView.h"
#include "View/ProcedureDialog/ProcedureFields/IFiberSplintView.h"
#include "View/ProcedureDialog/CommonFields/ICommonFields.h"

class IProcedureEditDialog
{

public:

	virtual void setMtype(ProcedureType m) = 0;
	virtual ICommonFields* commonFields() = 0;
	virtual ICrownView* crownView() = 0;
	virtual IObturationView* obturationView() = 0;
	virtual IImplantView* implantView() = 0;
	virtual IFiberSplintView* fiberView() = 0;
	virtual void closeDialog() = 0;
};