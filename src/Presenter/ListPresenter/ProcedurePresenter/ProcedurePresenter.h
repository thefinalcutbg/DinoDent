#pragma once

#include <vector>
#include <array>

#include "../Editor.h"
#include "Model/Tooth/Tooth.h"
#include "Model/Manipulation/Manipulation.h"

class IProcedureView;
class AmbList;
class Patient;

class ProcedurePresenter : public Editor
{
	AmbList* _ambList;
	Patient* _patient;
	std::vector<Tooth*>* _selectedTeeth;

	int _index;

	IProcedureView* view;

	void addToProcedureList(const std::vector<Manipulation>& new_mList);
	void refreshProcedureView();

public:
	ProcedurePresenter();

	void setData(AmbList &amb_list, Patient& patient, std::vector<Tooth*>& selectedTeeth);
	void setView(IProcedureView* view);

	void addProcedure();
	void editProcedure();
	void deleteProcedure(int index);
	void setSelectedProcedure(int index);
	void setUnfavourable(bool unfav);
};

