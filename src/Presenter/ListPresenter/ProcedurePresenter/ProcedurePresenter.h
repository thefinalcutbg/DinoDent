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
	std::vector<int>* _selectedIndexes;

	IProcedureView* view;

	void addToProcedureList(const std::vector<Manipulation>& new_mList);
	void refreshProcedureView();
	std::vector<Tooth*> getSelectedTeethPointers();

public:
	ProcedurePresenter();

	void setData(AmbList &amb_list, Patient& patient, std::vector<int>& selectedIndexes);
	void setView(IProcedureView* view);

	void addProcedure();
	void deleteProcedure(int index);
	void setSelectedProcedure(int index);
	void setUnfavourable(bool unfav);
};

