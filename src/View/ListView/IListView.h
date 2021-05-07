#pragma once

#include <vector>

#include "Model/AmbList.h"
#include "Model/Patient.h"
#include "Model/CheckState.h"
#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"
#include "Procedures/RowData.h"
#include "ToothPaintDevices/PaintHint.h"

class IListView : public CheckModelReciever 
{
public:
	virtual void refresh(AmbList& ambList, Patient &patient, const std::array<PaintHint, 32>& teeth, std::vector<int>& selectedIndexes) = 0;
	virtual void repaintTooth(const PaintHint& tooth) = 0;
	virtual void repaintBridges(const std::array<BridgeAppearenceTuple, 32>& bridges) = 0;
	virtual void updateControlPanel(const Tooth* tooth) = 0;
	virtual void openProcedureDialog(ProcedureDialogPresenter *p) = 0;
	virtual void setManipulations(const std::vector<RowData>& m) = 0;
};