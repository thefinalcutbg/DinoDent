#include "ListPresenter.h"

ListPresenter::ListPresenter(IListView* view) :
	view(view),
	ambList(NULL),
	patient(NULL),
	selectedIndexes(NULL),
	patientDialog(NULL),
	edit_observer(NULL),
	allergiesDialog(NULL),
	edited(NULL)
{
	checkModelCreator.addReciever(&statusControl);
	checkModelCreator.addReciever(view);
}

void ListPresenter::setDialogPresnters(AllergiesDialogPresenter* allergiesPresenter)
{
	allergiesDialog = allergiesPresenter;

}

void ListPresenter::attachEditObserver(EditObserver* observer)
{
	edit_observer = observer;
}


void ListPresenter::setPatientDialog(PatientDialogPresenter* patientDialogPresenter)
{
	patientDialog = patientDialogPresenter;
}


void ListPresenter::setData(ListInstance* listInstance)
{
	this->ambList = &listInstance->amb_list;
	this->patient = &listInstance->patient;
	this->edited = &listInstance->edited;
	this->selectedIndexes = &listInstance->selectedIndexes;

	selectionManager.setStatus(&ambList->teeth);

	setSelectedTeeth(listInstance->selectedIndexes);
	
	view->refresh
	(
		*this->ambList, 
		*this->patient, 
		paint_hint_generator.getTeethHint(ambList->teeth), 
		*selectedIndexes
	);

	refreshManipulationView(ambList->manipulations);
	view->repaintBridges(paint_hint_generator.statusToUIBridge(ambList->teeth));
}

void ListPresenter::setSelectedTeeth(std::vector<int>& SelectedIndexes)
{
	*this->selectedIndexes = SelectedIndexes;


	auto selectedTeeth = selectionManager.getSelectedTeethPointers(SelectedIndexes);
	statusControl.setSelectedTeeth(selectedTeeth);
	checkModelCreator.refreshModel(selectedTeeth);

	if (selectedIndexes->size() == 1) {
		view->updateControlPanel(&ambList->teeth[selectedIndexes->at(0)]);
	}
	else {
		view->updateControlPanel(NULL);
	}
}

void ListPresenter::makeEdited()
{
	if (*edited) return;
	*edited = true;
	if (edit_observer != NULL)
		edit_observer->notify();
}

void ListPresenter::statusChanged()
{
	makeEdited();

	checkModelCreator.refreshModel(selectionManager.getSelectedTeethPointers(*selectedIndexes));

	for (int i : *selectedIndexes)
	{
		
		view->repaintTooth(paint_hint_generator.getToothHint(ambList->teeth[i]));
	}

	view->repaintBridges(paint_hint_generator.statusToUIBridge(ambList->teeth));

	if (selectedIndexes->size() == 1) {
		view->updateControlPanel(&ambList->teeth[selectedIndexes->at(0)]);
	}
	else {
		view->updateControlPanel(NULL);
	}
}

void ListPresenter::addToManipulationList(const std::vector<Manipulation>& new_mList)
{
	auto& mList = ambList->manipulations;
	mList.reserve(mList.size() + new_mList.size());

	for (int i = 0; i < mList.size(); i++)
	{
		if (new_mList[0].date < mList[i].date)
		{
			mList.insert(mList.begin() + i, new_mList.begin(), new_mList.end());
			return;
		}
	}

	//if list is empty, or date is max
	for (auto &mInsert : new_mList)
	{
		mList.push_back(mInsert);
	}
		

}

void ListPresenter::refreshManipulationView(const std::vector<Manipulation>& mList)
{
	std::vector<RowData> rows;
	rows.reserve(mList.size());

	for (auto& m : mList)
	{
		int tooth = m.tooth;
		if (tooth >= 0 && tooth < 32)
		{
		    tooth = ToothUtils::getToothNumber(tooth, ambList->teeth[tooth].temporary.exists());
		}

		rows.emplace_back
		(
			RowData
			{
				Date::toString(m.date),
				m.diagnosis,
				tooth,
				m.name,
				m.code,
				m.price,
				m.nzok
			}
		);
	}

	view->setManipulations(rows);
}


void ListPresenter::changeStatus(Surface surface, SurfaceType type)
{
	statusControl.changeStatus(surface, type);
	statusChanged();
}

void ListPresenter::changeStatus(StatusAction status)
{
	if (status == StatusAction::addManipulation)
	{
		addProcedure(); return;
	}

	statusControl.changeStatus(status);

	if (status == StatusAction::Bridge || status == StatusAction::Crown) {
		bridgeController.formatBridges(*selectedIndexes, &ambList->teeth);
	}

	statusChanged();
}



void ListPresenter::openPatientDialog()
{
	if (patientDialog == NULL) return;

	patientDialog->open(this, *patient);
}

void ListPresenter::openAllergiesDialog()
{
	if (allergiesDialog == NULL) return;

	allergiesDialog->openDialog(this, *patient);

}

void ListPresenter::addProcedure()
{
	
	auto mNzokTemplate = getNZOK(Patient::getAge(patient->birth), 64, false);
	auto mCustomTemplate = getCustomManipulations();
	mNzokTemplate.insert(mNzokTemplate.end(), mCustomTemplate.begin(), mCustomTemplate.end());

	auto selectedTeethPtr = selectionManager.getSelectedTeethPointers(*selectedIndexes);

	ProcedureDialogPresenter p
	{
		mNzokTemplate,
		selectedTeethPtr,
		ambList->teeth,
		ambList->date 
	};

	view->openProcedureDialog(&p);

	auto newList = p.getManipulations();

	if (!newList.empty())
	{
		this->addToManipulationList(newList);
		refreshManipulationView(ambList->manipulations);
		makeEdited();
	}
	
}

void ListPresenter::deleteProcedure(int index)
{
	if (ambList->manipulations.size())
		makeEdited();

	ambList->manipulations.erase(ambList->manipulations.begin() + index);
	refreshManipulationView(ambList->manipulations);
}

void ListPresenter::statusChanged(std::string status)
{
	ambList->test = status;

	makeEdited();
}

void ListPresenter::manipulationSelected(int index)
{
}

void ListPresenter::setPatient(Patient patient)
{
	*this->patient = patient;
	view->refresh
	(
		*this->ambList,
		*this->patient,
		paint_hint_generator.getTeethHint(ambList->teeth),
		*selectedIndexes
	);
}

void ListPresenter::setAllergies(Allergies allergies)
{
	this->patient->allergies = allergies.allergies;
	this->patient->currentDiseases = allergies.current;
	this->patient->pastDiseases = allergies.past;

	view->refresh
	(
		*this->ambList,
		*this->patient,
		paint_hint_generator.getTeethHint(ambList->teeth),
		*selectedIndexes
	);
}
