#pragma once
#include "Model/Dental/Procedure.h"

#include "View/Interfaces/IImplantView.h"
#include "View/Interfaces/IObturationView.h"
#include "View/Interfaces/ICrownView.h"
#include "View/Interfaces/IFiberSplintView.h"
#include "View/Interfaces/ICommonFields.h"

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