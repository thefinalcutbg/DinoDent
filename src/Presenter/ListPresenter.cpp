#include "ListPresenter.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/User.h"
#include "Database/DbAmbList.h"
#include "Model/Validators/AmbListValidator.h"
#include "View/ModalDialogBuilder.h"
#include "View/Printer.h"
#include "Presenter/PatientDialogPresenter.h"
#include "Presenter/AllergiesDialogPresenter.h"
#include "Presenter/ProcedureDialogPresenter.h"
#include "View/ModalDialogBuilder.h"

#include "Presenter/TabPresenter.h"
#include "Database/DbPatient.h"


ListPresenter::ListPresenter(ITabView* tabView, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient, long long rowId)
    :
    TabInstance(tabView, TabType::AmbList, patient),
    view(tabView->listView()),
    tabPresenter(tabPresenter),
    m_ambList(rowId ? DbAmbList::getListData(rowId) : DbAmbList::getNewAmbSheet(patient->rowid)),
    patient_info(tabView->listView()->tileInfo(), patient)
{

    surf_presenter.setStatusControl(this);

    auto ambSheetDate = m_ambList.getDate();

    //the list is not new
    if (rowId) return;

    m_ambList.number = DbAmbList::getNewNumber(m_ambList.getDate(), m_ambList.hasNZOKProcedure());

    if (!User::practice().nzok_contract) return;

    if(User::settings().getPisHistoryAuto) requestPisActivities();


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

void ListPresenter::refreshPrices()
{
    double patientPrice(0);
    double nzokPrice(0);


    for (auto& m : m_ambList.procedures.list())
    {
        patientPrice = patientPrice + m.price;

        if (m.nhif)
        {
            auto [p, nhif] = NhifProcedures::getPrices(m.code, m_ambList.getDate(), patient->isAdult(m.date), User::doctor().specialty, m_ambList.nhifData.specification);
            nzokPrice = nzokPrice + nhif;
        }

    }

    view->refreshPriceLabel(patientPrice, nzokPrice);
}


bool ListPresenter::isValid()
{
    if (m_ambList.hasNZOKProcedure() && !patient->PISHistory.has_value()) {
        ModalDialogBuilder::showMessage(
            u8"Не са заредени данни от ПИС. "
            u8"Листът ще бъде валидиран само "
            u8"спрямо съществуващите записи в локалната база данни");
    }


    AmbListValidator checker(m_ambList, *patient);

    if (checker.ambListIsValid()) return true;

    ModalDialogBuilder::showError(checker.getErrorMsg());

    return false;
}



void ListPresenter::chargeChanged(int index)
{
    m_ambList.nhifData.charge = static_cast<NhifCharge>(index);
    makeEdited();
}

TabName ListPresenter::getTabName()
{
    TabName n;


    n.header += m_ambList.isNew() ? u8"Нов амб.лист" :
        u8"Амб.лист №" + std::to_string(m_ambList.number);

   
    n.footer = patient->FirstName;
    n.footer += " ";
    n.footer += patient->LastName;

    n.nhif = m_ambList.hasNZOKProcedure();

    return n;
}

long long ListPresenter::rowID() const
{
    return m_ambList.rowid;
}

bool ListPresenter::save()
{
    if (!requiresSaving()) return true;

    if (!isValid()) return false;

    auto d = m_ambList.getDate();

    if (DbAmbList::suchNumberExists(d.year, m_ambList.number, m_ambList.rowid) &&
        !ModalDialogBuilder::askDialog(
            u8"Амбулаторен лист с такъв номер вече съществува. "
            u8"Сигурни ли сте че искате да дублирате номерацията?"))
    {
        return false;
    }

    if (m_ambList.isNew())
    {
        m_ambList.rowid = DbAmbList::insert(m_ambList, patient->rowid);
    }
    else
    {
        DbAmbList::update(m_ambList);
    }

    edited = false;

    refreshTabName();

    return true;

}

bool ListPresenter::isNew()
{
    return m_ambList.isNew();
}

void ListPresenter::print()
{
    if(save())
    Print::ambList(m_ambList, *patient);
}

void ListPresenter::setDataToView()
{
    
    view->setPresenter(this);
    patient_info.setDate(m_ambList.getDate());
    patient_info.setCurrent();

    view->refresh(m_ambList);

    view->setAmbListNum(m_ambList.number);

    surf_presenter.setStatusControl(this);
    surf_presenter.setView(view->surfacePanel());
    view->surfacePanel()->setPresenter(&surf_presenter);

    auto hints = ToothHintCreator::getTeethHint(m_ambList.teeth);
    for (auto& hint : hints)
    {
        view->repaintTooth(hint);
    }

    view->setNotes(patient->teethNotes);
    
    view->setSelectedTeeth(m_selectedIndexes);

    showCurrentStatus(m_showCurrentStatus);

    refreshPrices();
    refreshProcedureView();

   
}


void ListPresenter::ambNumChanged(long long value)
{
    m_ambList.number = value;
    edited = false;
    makeEdited();
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

        for (int i = 0; i < 32; i++){
            view->repaintTooth(ToothHintCreator::getToothHint(m_ambList.teeth[i]));
        }
    }
    else if (code == StatusCode::Temporary){ 
        refreshPrices();
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

int ListPresenter::generateAmbListNumber()
{
    int newNumber = m_ambList.number;

    auto ambSheetDate = m_ambList.getDate();

    if (m_ambList.isNew() || m_ambList.hasNumberInconsistency()) {
        newNumber = DbAmbList::getNewNumber(ambSheetDate, m_ambList.hasNZOKProcedure());
    }

    return newNumber;
}

void ListPresenter::requestPisActivities()
{
    dentalActService.sendRequest(patient->type, patient->id,
        [&](auto procedures) { setPISActivities(procedures);}
    );

}

void ListPresenter::setPISActivities(const std::optional<std::vector<Procedure>>& pisProcedures)
{

    if (!pisProcedures.has_value()) {
        m_openHistoryDialogOnReply = false;
        return;
    }

    patient->PISHistory = pisProcedures;

    if (m_openHistoryDialogOnReply) openPisHistory();
}

#include "Presenter/DetailedStatusPresenter.h"

void ListPresenter::openPisHistory()
{

    if (!patient->PISHistory.has_value())
    {
        m_openHistoryDialogOnReply = true;
        requestPisActivities();
        return;
    }

    m_openHistoryDialogOnReply = false;

    auto& history = patient->PISHistory.value();

    if (history.empty()) {
        ModalDialogBuilder::showMessage(u8"В ПИС не са намерени данни за този пациент");
        return;
    }

    bool applyToStatus = ModalDialogBuilder::pisHistoryDialog(history);

    if (!applyToStatus) return;
    
                for (auto it = history.rbegin(); it != history.rend(); ++it)
                it->applyPISProcedure(m_ambList.teeth);

                for (auto& t : m_ambList.teeth)
                view->repaintTooth(ToothHintCreator::getToothHint(t)); 
   

    makeEdited();
}

void ListPresenter::openDetails(int toothIdx)
{
    DetailedStatusPresenter d(m_ambList.teeth[toothIdx], patient->rowid);

    auto result = d.open();

    if (!result.has_value()) return;
    
    m_ambList.teeth.setToothDetails(result.value());

    patient->teethNotes[toothIdx] = d.getNote();

    view->setNotes(patient->teethNotes);
   
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
    m_ambList.procedures.addProcedures(new_mList);
}

void ListPresenter::refreshProcedureView()
{
    if (view == nullptr) return;

    auto& mList = m_ambList.procedures;

    m_ambList.procedures.refreshTeethTemporary(m_ambList.teeth);
    view->setDateTime(m_ambList.getDate(), m_ambList.time);
    view->setProcedures(m_ambList.procedures.list());
}

void ListPresenter::addProcedure()
{
    ProcedureDialogPresenter p
    {
        m_ambList,
        m_selectedTeeth,
        patient->turns18At()
    };

    auto procedures = p.openDialog();

    if (procedures.empty()) return;

    this->addToProcedureList(procedures);

    if (m_ambList.hasNumberInconsistency()) {

        m_ambList.number = DbAmbList::getNewNumber(m_ambList.getDate(), m_ambList.hasNZOKProcedure());
        
        if (procedures[0].nhif) view->setNhifData(m_ambList.nhifData);
        
    }

    if (view == nullptr) return;

    view->setAmbListNum(m_ambList.number);

    patient_info.setDate(m_ambList.getDate());
    refreshPrices();
    refreshProcedureView();

    edited = false;
    makeEdited();

}

#include "Presenter/ProcedureEditorPresenter.h"

void ListPresenter::editProcedure(int index)
{
    if (index < 0 || index >= m_ambList.procedures.size()) return;

    auto& m_for_edit = m_ambList.procedures.at(index);

    ProcedureEditorPresenter p(m_for_edit, patient->turns18At());

    auto result = p.openDialog();

    if (!result) return;

    auto& m = result.value();

    m_ambList.procedures.replaceProcedure(m, index);

    patient_info.setDate(m_ambList.getDate());
    refreshPrices();
    refreshProcedureView();
    makeEdited();

}

void ListPresenter::deleteProcedure(int index)
{
    m_ambList.procedures.removeProcedure(index);

    if (m_ambList.hasNumberInconsistency()) {
        m_ambList.number = DbAmbList::getNewNumber(m_ambList.getDate(), m_ambList.hasNZOKProcedure());
        
        if (!m_ambList.hasNZOKProcedure()) {
            view->hideNhifSheetData();
        }
    }

    view->setAmbListNum(m_ambList.number);

    patient_info.setDate(m_ambList.getDate());
    refreshPrices();
    refreshProcedureView();

    edited = false;
    makeEdited();
}

void ListPresenter::setNhifData(const NhifSheetData& data)
{
    m_ambList.nhifData = data;
    refreshPrices();
    makeEdited();
}

void ListPresenter::createInvoice()
{
    auto selectedProcedures = ModalDialogBuilder::selectProcedures(m_ambList.procedures.list(), SelectionPref::OnlyPaid);

    if (!selectedProcedures.has_value()) {
        return;
    }

    tabPresenter->openInvoice(selectedProcedures.value(), patient->rowid);
}

void ListPresenter::createPerioMeasurment()
{
    tabPresenter->openPerio(*this->patient.get());
}

void ListPresenter::showCurrentStatus(bool show)
{
    view->disableGraphicsView(show);

    m_showCurrentStatus = show;

    auto currentStatus = m_ambList.teeth;

    for (auto& p : m_ambList.procedures.list())
    {
        p.applyProcedure(currentStatus);
    }

    if (show)
    {
        for (auto& t : currentStatus)
        {
            view->repaintTooth(ToothHintCreator::getToothHint(t));
        }
    }
    else
    {
        for (auto& t : m_ambList.teeth)
        {
            view->repaintTooth(ToothHintCreator::getToothHint(t));
        }
    }
}

ListPresenter::~ListPresenter()
{
    if (view) view->setPresenter(nullptr);
}
