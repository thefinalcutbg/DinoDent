#pragma once
#include <vector>
#include "Model/Procedure/Procedure.h"

class ProcedureDialogPresenter;
class ProcedurePresenter;
class AbstractComboBox;
class IProcedureView
{
public:
	virtual void setProcedures(const std::vector<Procedure>& m, double patientPrice, double nzokPrice) = 0;
	virtual void setUnfav(bool unfav) = 0;
	virtual AbstractComboBox* taxCombo() = 0;
	virtual void setProcedurePresenter(ProcedurePresenter* presenter) = 0;

};