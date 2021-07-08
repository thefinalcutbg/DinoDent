#pragma once

#include <vector>
#include <array>

#include "../Editor.h"
#include "Model/Tooth/Tooth.h"
#include "Model/Procedure/Procedure.h"

class IProcedureView;
class AmbList;
class Patient;

class ProcedurePresenter : public Editor
{
	AmbList* _ambList;
	Patient* _patient;
	std::vector<Tooth*>* _selectedTeeth;

	int index_;

	IProcedureView* view;

	void addToProcedureList(const std::vector<Procedure>& new_mList);
	void refreshProcedureView();

public:
	ProcedurePresenter();

	void setData(AmbList &amb_list, Patient& patient, std::vector<Tooth*>& selectedTeeth);
	void setView(IProcedureView* view);

	void addProcedure();
	void editProcedure();
	void deleteProcedure(int index);
	void ambDateChanged(Date date);
	void setSelectedProcedure(int index);
	void setUnfavourable(bool unfav);
};

