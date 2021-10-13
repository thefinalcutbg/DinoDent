#include "ListPresenter.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Model/User/UserManager.h"
#include "Database/DbAmbList.h"
#include "Model/AmbListValidator.h"
#include "View/ErrorMessage.h"
#include "View/Printer/Printer.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "Presenter/AllergiesDialog/AllergiesDialogPresenter.h"
#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"
#include "View/ModalDialogBuilder.h"


bool ListPresenter::isValid()
{
    AmbListValidator checker(m_ambList, *patient);

    if(checker.ambListIsValid())
        return true;

    showError(checker.getErrorMsg());

    return false;
}


ListPresenter::ListPresenter(ITabView* tabView, std::shared_ptr<Patient> patient) :
    TabInstance(tabView, TabType::AmbList),
    view(tabView->listView()),
    patient(patient),
    m_selectedProcedure(-1),
    m_ambList(db.getListData(patient->id, Date::currentMonth(), Date::currentYear()))
{
    surf_presenter.setStatusControl(this);

    if (m_ambList.isNew() && !patient->isAdult(m_ambList.date))
        m_ambList.charge = Charge::freed;
    else if (m_ambList.isNew() && patient->getAge(m_ambList.date) > 70)
       m_ambList.charge = Charge::retired;

    for (auto& m : m_ambList.procedures) //autofill NZOK procedures
        if (m.nzok)
            m.price = MasterNZOK::instance().getPatientPrice(m.code, m_ambList.date, UserManager::currentUser().specialty, patient->isAdult(), m_ambList.full_coverage);
}

ListPresenter::ListPresenter(ITabView* tabView, std::shared_ptr<Patient> patient, const std::string& ambListId)
    :
    TabInstance(tabView, TabType::AmbList),
    view(tabView->listView()),
    patient(patient),
    m_ambList(db.getListData(ambListId)),
    m_selectedProcedure(-1)
{
    surf_presenter.setStatusControl(this);

    if (m_ambList.isNew() && !patient->isAdult(m_ambList.date))
        m_ambList.charge = Charge::freed;
    else if (m_ambList.isNew() && patient->getAge(m_ambList.date) > 70)
        m_ambList.charge = Charge::retired;

    for (auto& m : m_ambList.procedures) //autofill NZOK procedures
        if (m.nzok)
            m.price = MasterNZOK::instance().getPatientPrice(m.code, m_ambList.date, UserManager::currentUser().specialty, patient->isAdult(), m_ambList.full_coverage);
}

void ListPresenter::statusChanged()
{
    m_checkModel = CheckModel(m_ambList.teeth.getSelectedTeethPtr(m_selectedIndexes));

    view->setCheckModel(m_checkModel);;

    for (auto& t : m_selectedTeeth)
    {
        view->repaintTooth(ToothHintCreator::getToothHint(*t));
    }

    if (m_selectedIndexes.size() == 1)
        surf_presenter.setTooth(&m_ambList.teeth[m_selectedIndexes[0]]);

    makeEdited();
}




void ListPresenter::chargeChanged(int index)
{
    m_ambList.charge = static_cast<Charge>(index);
    makeEdited();
}

std::string ListPresenter::getTabName()
{
    std::string tabName;
    tabName.reserve(60);

    if (edited) tabName += "*";

    tabName += m_ambList.isNew() ? u8"Нов амб.лист" :
        u8"Амб.лист №" + std::to_string(m_ambList.number);

    tabName += " ";
    tabName += patient->FirstName;
    tabName += " ";

    tabName += patient->LastName;

    return tabName;
}

bool ListPresenter::save()
{
    if (m_ambList.isNew()) return saveAs();

    if (!isValid()) return false;

    if (edited) {  db.updateAmbList(m_ambList);}

    edited = false;
   // listSelector_.refreshModel();
    _tabView->changeTabName(getTabName());


}

bool ListPresenter::saveAs()
{
    int newNumber = 0;

    auto map = db.getExistingNumbers(m_ambList.date.year);

    if (!m_ambList.number) {
        newNumber = db.getNewNumber(m_ambList.date.year);
    }
    else {
        newNumber = m_ambList.number;
        map[newNumber] = false;
    }
    newNumber = ModalDialogBuilder::openSaveAsDialog(newNumber, map);

    if (!newNumber) return false;

    m_ambList.number = newNumber;

    if (m_ambList.isNew()) {
        db.insertAmbList(m_ambList, patient->id);
    }
    else {
        db.updateAmbList(m_ambList);
    }

  //  listSelector_.refreshModel();

    edited = false;

    _tabView->changeTabName(getTabName());

    return true;
}

bool ListPresenter::isNew()
{
    return m_ambList.isNew();
}

void ListPresenter::print()
{
    Print::ambList(m_ambList, *patient);
}

void ListPresenter::setCurrent()
{
    
    view->setPresenter(this);
    

    view->refresh
    (
        m_ambList,
        *patient.get()
    );

    surf_presenter.setStatusControl(this);
    surf_presenter.setView(view->surfacePanel());
    view->surfacePanel()->setPresenter(&surf_presenter);

    auto hints = ToothHintCreator::getTeethHint(m_ambList.teeth);
    for (auto& hint : hints)
    {
        view->repaintTooth(hint);
    }
    
    view->setSelectedTeeth(m_selectedIndexes);

    refreshProcedureView();

    _tabView->showListView();
    setScrollPosition();
    
}


void ListPresenter::openPatientDialog()
{

    PatientDialogPresenter p{ *patient };
  
    auto patient = p.open();

    if (!patient.has_value()) return;

    *this->patient = patient.value();


    view->refresh
    (
        m_ambList,
        *this->patient.get()
    );

}

void ListPresenter::openAllergiesDialog()
{
    AllergiesDialogPresenter p(*patient.get());

    auto data = p.openDialog();

    if (!data.has_value()) return;

    auto& d = data.value();

    patient->allergies = d.allergies;
    patient->currentDiseases = d.current;
    patient->pastDiseases = d.past;

    view->refresh
    (
        m_ambList,
        *patient.get()
    );
}

void ListPresenter::setCaries(int surface)
{
    bool state = m_checkModel.cariesStatus[surface] != CheckState::checked;

    for (auto& t : m_selectedTeeth) t->setStatus(StatusType::caries, surface, state);

    statusChanged();
}

void ListPresenter::setObturation(int surface)
{
    bool state = m_checkModel.obturationStatus[surface] != CheckState::checked;

    for (auto& t : m_selectedTeeth) t->setStatus(StatusType::obturation, surface, state);

    statusChanged();
}

void ListPresenter::setMainStatus(int code)
{
    bool state = m_checkModel.generalStatus[code] != CheckState::checked;

    for (auto& t : m_selectedTeeth)
    {
        t->setStatus(StatusType::general, code, state);
    }

    if (code == StatusCode::Bridge || code == StatusCode::Crown || code == StatusCode::FiberSplint) {
        m_ambList.teeth.formatBridges(m_selectedIndexes);
        for (int i = 0; i < 32; i++)
        {
            view->repaintTooth(ToothHintCreator::getToothHint(m_ambList.teeth[i]));
        }
    }
    else if (code == StatusCode::Temporary){ 
        refreshProcedureView(); //updates the teeth num
    }

    statusChanged();
}

void ListPresenter::setOther(int code)
{
    auto DO = [](Tooth* t)
    {
        t->removeStatus(StatusCode::Obturation);
        t->setStatus(StatusType::obturation, Surface::Distal);
        t->setStatus(StatusType::obturation, Surface::Occlusal);
    };

    auto MO = [](Tooth* t)
    {
        t->removeStatus(StatusCode::Obturation);
        t->setStatus(StatusType::obturation, Surface::Medial);
        t->setStatus(StatusType::obturation, Surface::Occlusal);
    };

    auto MOD = [](Tooth* t)
    {
        t->removeStatus(StatusCode::Obturation);
        t->setStatus(StatusType::obturation, Surface::Medial);
        t->setStatus(StatusType::obturation, Surface::Distal);
        t->setStatus(StatusType::obturation, Surface::Occlusal);
    };

    auto& teeth = m_ambList.teeth;

    switch (code)
    {
    case OtherInputs::DO: for (auto& t : m_selectedTeeth)DO(t); break;
    case OtherInputs::MO: for (auto& t : m_selectedTeeth)MO(t); break;
    case OtherInputs::MOD: for (auto& t : m_selectedTeeth)MOD(t); break;
    case OtherInputs::removeC: for (auto& t : m_selectedTeeth) t->removeStatus(StatusType::caries); break;
    case OtherInputs::removeO: for (auto& t : m_selectedTeeth) t->removeStatus(StatusType::obturation); break;
    case OtherInputs::removeBridge: 
        for (auto& t : m_selectedTeeth) teeth.removeBridge(t->index);  
        teeth.formatBridges(m_selectedIndexes);         
        for (int i = 0; i < 32; i++)
        {
            view->repaintTooth(ToothHintCreator::getToothHint(m_ambList.teeth[i]));
        } 
        break;
    case OtherInputs::removeAll: for (auto& t : m_selectedTeeth) t->removeStatus();  teeth.formatBridges(m_selectedIndexes); break;
    }

    statusChanged();
}

void ListPresenter::setSelectedTeeth(const std::vector<int>& SelectedIndexes)
{
    m_selectedIndexes = SelectedIndexes;
    m_selectedTeeth = m_ambList.teeth.getSelectedTeethPtr(SelectedIndexes);
    m_checkModel = CheckModel(m_selectedTeeth);
    view->setCheckModel(m_checkModel);

    bool oneToothSelected = m_selectedTeeth.size() == 1;

    if (oneToothSelected)
    {
        surf_presenter.setTooth(m_selectedTeeth[0]);
    }
    
    view->hideControlPanel(!m_selectedTeeth.size());
    view->hideSurfacePanel(!oneToothSelected);
}

#include "Presenter/DetailsPresenter/DetailedStatusPresenter.h"

void ListPresenter::openDetails(int toothIdx)
{
    DetailedStatusPresenter d(m_ambList.teeth[toothIdx], patient->id);

    auto result = d.open();

    if (!result.has_value()) return;
    
    m_ambList.teeth.setToothDetails(result.value());
   
    for (int i = 0; i < 32; i++)
    {
        view->repaintTooth(ToothHintCreator::getToothHint(m_ambList.teeth[i]));
    }

    statusChanged();
}

void ListPresenter::openDetails()
{
    if (m_selectedTeeth.size() == 1) openDetails(m_selectedTeeth[0]->index);
}


void ListPresenter::addToProcedureList(const std::vector<Procedure>& new_mList)
{
    auto& mList = m_ambList.procedures;

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
    for (auto& mInsert : new_mList)
    {
        mList.push_back(mInsert);
    }

}

void ListPresenter::refreshProcedureView()
{
    if (view == nullptr) return;

    auto& mList = m_ambList.procedures;

    double patientPrice(0);
    double nzokPrice(0);

    for (auto& m : mList)
    {
        int toothIndex(m.tooth);

        if (toothIndex >= 0 && toothIndex < 32)
        {
            m.temp = m_ambList.teeth[toothIndex].temporary.exists();
        }

        patientPrice = patientPrice + m.price;

        if (m.nzok)
        {
            auto [p, nzok] = MasterNZOK::instance().getPrices(m.code, m_ambList.date, UserManager::currentUser().specialty, patient->isAdult(m.date), m_ambList.full_coverage);
            nzokPrice = nzokPrice + nzok;
        }

    }

    view->setProcedures(mList, patientPrice, nzokPrice);
}

void ListPresenter::addProcedure()
{
    if (view == nullptr) return;

    ProcedureDialogPresenter p
    {
        m_selectedTeeth,
        m_ambList.teeth,
        m_ambList.date,
        patient->turns18At(),
        m_ambList.full_coverage,
        UserManager::currentUser().specialty
    };

    auto openList = p.openDialog();

    if (openList.empty()) return;

    if (openList[0].nzok) //re-getting the prices because of edge-case (18th birthday in current month)
    {
        for (auto& m : openList)
            m.price = MasterNZOK::instance().
            getPatientPrice
            (
                m.code, m_ambList.date,
                UserManager::currentUser().specialty,
                patient->isAdult(m.date),
                m_ambList.full_coverage
            );
    }


    this->addToProcedureList(openList);

    refreshProcedureView();
    makeEdited();

}

#include "Presenter/ProcedureDialog/ProcedureEditorPresenter.h"

void ListPresenter::editProcedure()
{
    if (m_selectedProcedure == -1) return;

    auto& m_for_edit = m_ambList.procedures.at(m_selectedProcedure);

    ProcedureEditorPresenter p(m_for_edit, m_ambList.date, patient->turns18At());

    auto result = p.openDialog();

    if (!result.has_value()) return;

    auto& m = result.value();

    if (m.nzok)
    {
        m.price = MasterNZOK::instance().
            getPatientPrice
            (
                m.code,
                m_ambList.date,
                UserManager::currentUser().specialty,
                patient->isAdult(m.date),
                m_ambList.full_coverage
            );

    }

    if (m.date == m_for_edit.date)
    {
        m_for_edit = m;
    }
    else
    {
        deleteProcedure(m_selectedProcedure);
        addToProcedureList(std::vector<Procedure>{m});
    }

    refreshProcedureView();
    makeEdited();

}

void ListPresenter::deleteProcedure(int index)
{
    if (!m_ambList.procedures.size()) return;

    m_ambList.procedures.erase(m_ambList.procedures.begin() + index);

    refreshProcedureView();
    makeEdited();
}

void ListPresenter::ambDateChanged(Date date)
{
    m_ambList.date = date;
    makeEdited();
}

void ListPresenter::setSelectedProcedure(int index)
{
    m_selectedProcedure = index;
}

void ListPresenter::setUnfavourable(bool unfav)
{
    m_ambList.full_coverage = unfav;

    for (auto& m : m_ambList.procedures)
    {
        if (m.nzok)
        {
            m.price = MasterNZOK::instance().
                getPatientPrice
                (
                    m.code,
                    m_ambList.date,
                    UserManager::currentUser().specialty,
                    patient->isAdult(m.date),
                    unfav
                );

        }
    }

    if (unfav)
    {
        view->taxCombo()->setIndex(2);
        m_ambList.charge = Charge::freed;
    }

    makeEdited();

    refreshProcedureView();
}

ListPresenter::~ListPresenter()
{
    if (view) view->setPresenter(nullptr);
}
