#pragma once
#include <vector>

#include "Model/Procedure/TableStructs.h"

class ProcedureDialogPresenter;
class ProcedurePresenter;
class AbstractComboBox;
class IProcedureView
{
public:
	virtual void setProcedures(const std::vector<ProcedureRowData>& m, double patientPrice, double nzokPrice) = 0;
	virtual void setUnfav(bool unfav) = 0;
	virtual AbstractComboBox* taxCombo() = 0;
	virtual void setProcedurePresenter(ProcedurePresenter* presenter) = 0;

};