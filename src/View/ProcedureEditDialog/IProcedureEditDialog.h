#pragma once

#include "Model/Manipulation/Manipulation.h"

#include "View/ProcedureDialog/Widgets/ObturationView/IObturationView.h"
#include "View/ProcedureDialog/Widgets/ImplantView/IImplantView.h"
#include "View/ProcedureDialog/Widgets/CrownView/ICrownView.h"

class ProcedureEditorPresenter;
class ICommonFields;
class CrownPresenter;
class ImplantPresenter;
class ObturationPresenter;

class IProcedureEditDialog
{

public:

	virtual void setMtype(ManipulationType m) = 0;
	virtual ICommonFields* commonFields() = 0;
	virtual ICrownView* crownView() = 0;
	virtual IObturationView* obturationView() = 0;
	virtual IImplantView* implantView() = 0;
	virtual void closeDialog() = 0;
};