#pragma once

#include <vector>

#include "Model/AmbList.h"
#include "Model/Patient.h"
#include "Model/CheckState.h"
#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"
#include "Procedures/RowData.h"
#include "ToothPaintDevices/PaintHint.h"

#include "IStatusView.h"

class IListView : public IStatusView
{
public:
	virtual void refresh(AmbList& ambList, Patient &patient) = 0;
	virtual void openProcedureDialog(ProcedureDialogPresenter *p) = 0;
	virtual void setManipulations(const std::vector<RowData>& m) = 0;
	virtual void setUnfav(bool unfav) = 0;
};