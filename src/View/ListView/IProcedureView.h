#pragma once
#include <vector>
#include "Procedures/RowData.h"

class ProcedureDialogPresenter;
class ProcedurePresenter;

class IProcedureView
{
public:
	virtual void setProcedures(const std::vector<RowData>& m) = 0;
	virtual void openProcedureDialog(ProcedureDialogPresenter* p) = 0;
	virtual void setUnfav(bool unfav) = 0;
	virtual void setProcedurePresenter(ProcedurePresenter* presenter) = 0;

};