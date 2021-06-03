#include "ListPresenter.h"
#include "Model/Manipulation/MasterNZOK.h"
#include "Model/User/User.h"

ListPresenter::ListPresenter() :
    view(nullptr),
    ambList(nullptr),
    patient(nullptr),
    patientDialog(nullptr),
    allergiesDialog(nullptr)
{


}

void ListPresenter::setDialogPresnters(AllergiesDialogPresenter* allergiesPresenter)
{
    allergiesDialog = allergiesPresenter;

}



void ListPresenter::setUnfavourable(bool unfav)
{

    ambList->unfavourable = unfav;

    for (auto& m : ambList->manipulations)
    {
        if (m.nzok)
        {
            m.price = std::get<0>(
                MasterNZOK::instance().getPrices
                (
                    m.code,
                    ambList->date,
                    CurrentUser::instance().specialty,
                    this->patient->isAdult(m.date),
                    unfav
                )
                );
        }
    }

    makeEdited();

    refreshManipulationView(ambList->manipulations);

}


void ListPresenter::setPatientDialog(PatientDialogPresenter* patientDialogPresenter)
{
    patientDialog = patientDialogPresenter;
}


void ListPresenter::setData(ListInstance* inst)
{

    this->ambList = &inst->amb_list;
    this->patient = &inst->patient;
   
    status_presenter.setData(inst->amb_list.teeth, inst->selectedIndexes);

    view->refresh
    (
        *this->ambList,
        *this->patient
    );

    view->setUnfav(ambList->unfavourable);

    refreshManipulationView(ambList->manipulations);

}

void ListPresenter::setView(IListView* view)
{
    this->view = view;
    status_presenter.setView(view);
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


void ListPresenter::attachEditObserver(EditObserver* observer)
{
    Editor::attachEditObserver(observer);
    status_presenter.attachEditObserver(observer);
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

    auto mNzokTemplate = MasterNZOK::instance().getM_Templates(ambList->date, CurrentUser::instance().specialty , patient->isAdult(), false);
    auto mCustomTemplate = getCustomManipulations();
    mNzokTemplate.insert(mNzokTemplate.end(), mCustomTemplate.begin(), mCustomTemplate.end());

    auto selectedTeethPtr = status_presenter.getSelectedTeethPointers();

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

void ListPresenter::manipulationSelected(int index)
{
}

void ListPresenter::setPatient(Patient patient)
{
    *this->patient = patient;
    view->refresh
    (
        *this->ambList,
        *this->patient
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
        *this->patient
    );
}
