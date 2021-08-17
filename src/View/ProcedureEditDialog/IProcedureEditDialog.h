#pragma once
#include "Model/Procedure/ProcedureTemplate.h"

#include "View/uiComponents/qt_derived/Widgets/IImplantView.h"
#include "View/uiComponents/qt_derived/Widgets/IObturationView.h"
#include "View/uiComponents/qt_derived/Widgets/ICrownView.h"
#include "View/ProcedureDialog/CommonFields/ICommonFields.h"

class IProcedureEditDialog
{

public:

	virtual void setMtype(ProcedureType m) = 0;
	virtual ICommonFields* commonFields() = 0;
	virtual ICrownView* crownView() = 0;
	virtual IObturationView* obturationView() = 0;
	virtual IImplantView* implantView() = 0;
	virtual void closeDialog() = 0;
};