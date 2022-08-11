#include "ListPresenter.h"
#include "Model/Procedure/MasterNZOK.h"
#include "Model/User/User.h"
#include "Database/DbAmbList.h"
#include "Model/AmbListValidator.h"
#include "View/ModalDialogBuilder.h"
#include "View/Printer/Printer.h"
#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "Presenter/AllergiesDialog/AllergiesDialogPresenter.h"
#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Model/AmbList.h"
#include "Presenter/TabPresenter/TabPresenter.h"



ListPresenter::ListPresenter(ITabView* tabView, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient, long long rowId)
    :
    TabInstance(tabView, TabType::AmbList, patient),
    view(tabView->listView()),
    tabPresenter(tabPresenter),
    m_ambList(rowId ? DbAmbList::getListData(rowId) : DbAmbList::getNewAmbSheet(patient->rowid))
{

    surf_presenter.setStatusControl(this);

    auto ambSheetDate = m_ambList.getDate();

    if (m_ambList.isNew() && !patient->isAdult(ambSheetDate))
        m_ambList.charge = Charge::freed;
    else if (m_ambList.isNew() && patient->getAge(ambSheetDate) > 70)
        m_ambList.charge = Charge::retired;

    //the list is not new
    if (rowId) return;

    m_ambList.number = DbAmbList::getNewNumber(m_ambList.getDate(), m_ambList.hasNZOKProcedure());

    if (!User::practice().nzok_contract) return;

    if(User::settings().getPisHistoryAuto) requestPisActivities();

    if (User::practice().nzok_contract->nra_pass.size() &&
        User::settings().getNraStatusAuto
    )
    {
        checkHealthInsurance(false);
    }


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


bool ListPresenter::isValid()
{
    if (m_ambList.hasNZOKProcedure() && !patient->PISHistory.has_value()) {
        ModalDialogBuilder::showMessage(
            u8"Не са заредени данни от ПИС. "
            u8"Листът ще бъде валидиран само "
            u8"спрямо съществуващите записи в локалната база данни");
    }


    AmbListValidator checker(m_ambList, *patient);

    if (checker.ambListIsValid())
        return true;

    ModalDialogBuilder::showError(checker.getErrorMsg());

    return false;
}



void ListPresenter::chargeChanged(int index)
{
    m_ambList.charge = static_cast<Charge>(index);
    makeEdited();
}

TabName ListPresenter::getTabName()
{
    std::string header;

    std::string footer;

    header += m_ambList.isNew() ? u8"Нов амб.лист" :
        u8"Амб.лист №" + std::to_string(m_ambList.number);

   ;
    footer = patient->FirstName;
    footer += " ";
    footer += patient->LastName;

    return {header, footer, m_ambList.hasNZOKProcedure()};
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
    

    view->refresh
    (
        m_ambList,
        *patient.get()
    );

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

    refreshProcedureView();

   
}


void ListPresenter::openPatientDialog()
{
    if (patient == nullptr) return;

    PatientDialogPresenter p{ *patient };
  
    auto patient = p.open();

    if (!patient.has_value()) return;

    *this->patient = patient.value();

    tabPresenter->refreshPatientTabNames(patient->rowid);

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
    if (patient->PISHistory.has_value()) return;
      
    //sending request to PIS

    dentalActService.sendRequest(patient->type, patient->id,

        [=](auto procedures) {
            if (this) this->setPISActivities(procedures);
        }
    );

}

void ListPresenter::setPISActivities(const std::optional<Procedures>& pisProcedures)
{

    if (!pisProcedures.has_value()) {
        ModalDialogBuilder::showError(u8"Неуспешна връзка със сървъра");
        m_openHistoryDialogOnReply = false;
        return;
    }

    patient->PISHistory = pisProcedures;

    if (m_openHistoryDialogOnReply) openPisHistory();
}

void ListPresenter::setInsuranceStatus(const std::optional<InsuranceStatus>& status_result)
{
    if (!status_result) {
        return;
    }

    patient->insuranceStatus = status_result.value();

    view->refresh(m_ambList, *patient.get());

    if (m_showInsuranceDialog) {
        m_showInsuranceDialog = false;
        ModalDialogBuilder::showMessage(status_result->getYearsText());
    }

}

#include "Presenter/DetailsPresenter/DetailedStatusPresenter.h"

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

void ListPresenter::checkHealthInsurance(bool showDialog)
{

    m_showInsuranceDialog = showDialog;

    if (patient->insuranceStatus) {
        setInsuranceStatus(patient->insuranceStatus.value());
        return;
    }

    nraStatusServ.sendRequest(*patient.get(),

        [=](auto status){if(this)this->setInsuranceStatus(status);}

    );

}

void ListPresenter::checkDiagnosisNhif()
{
    nhifDiagnosisServ.sendRequest(
        patient->type,
        patient->id,

        //lamest call ever
        [=](const std::string& currentDiseases) {

            if (currentDiseases.empty()) {
                ModalDialogBuilder::showMessage(u8"Няма данни в рецептурната книжка");
                return;
            }

            Patient patient = *this->patient;
            patient.currentDiseases = currentDiseases;
            AllergiesDialogPresenter p(patient);
            ModalDialogBuilder::openDialog(&p);
            *this->patient = patient;
            view->refresh(m_ambList, patient);
        }
    );
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

    double patientPrice(0);
    double nzokPrice(0);

    m_ambList.procedures.refreshTeethTemporary(m_ambList.teeth);

    for (auto& m : m_ambList.procedures.list())
    {
        patientPrice = patientPrice + m.price;

        if (m.nzok)
        {
            auto [p, nzok] = MasterNZOK::instance().getPrices(m.code, m_ambList.getDate(), User::doctor().specialtyAsInt(), patient->isAdult(m.date), m_ambList.full_coverage);
            nzokPrice = nzokPrice + nzok;
        }

    }

    view->setProcedures(m_ambList.procedures.list(), patientPrice, nzokPrice);
}

void ListPresenter::addProcedure()
{


    ProcedureDialogPresenter p
    {
        m_ambList,
        m_selectedTeeth,
        patient->turns18At()
    };

    auto openList = p.openDialog();

    if (openList.empty()) return;

    /*
    if (openList[0].nzok) //re-getting the prices because of edge-case (18th birthday in current month)
    {
        for (auto& m : openList)
            m.price = MasterNZOK::instance().
            getPatientPrice
            (
                m.code, m_ambList.getAmbListDate(),
                User::currentUser().doctor.specialty,
                patient->isAdult(m.date),
                m_ambList.full_coverage
            );
    }
    */

    this->addToProcedureList(openList);

    if (m_ambList.hasNumberInconsistency()) {
        m_ambList.number = DbAmbList::getNewNumber(m_ambList.getDate(), m_ambList.hasNZOKProcedure());
    }

    if (view == nullptr) return;

    view->setAmbListNum(m_ambList.number);

    refreshProcedureView();

    edited = false;
    makeEdited();

}

#include "Presenter/ProcedureDialog/ProcedureEditorPresenter.h"

void ListPresenter::editProcedure(int index)
{
    if (index < 0 || index >= m_ambList.procedures.size()) return;

    auto& m_for_edit = m_ambList.procedures.at(index);

    ProcedureEditorPresenter p(m_for_edit, patient->turns18At());

    auto result = p.openDialog();

    if (!result) return;

    auto& m = result.value();

    m_ambList.procedures.replaceProcedure(m, index);

    refreshProcedureView();
    makeEdited();

}

void ListPresenter::deleteProcedure(int index)
{
    m_ambList.procedures.removeProcedure(index);

    if (m_ambList.hasNumberInconsistency()) {
        m_ambList.number = DbAmbList::getNewNumber(m_ambList.getDate(), m_ambList.hasNZOKProcedure());
    }

    view->setAmbListNum(m_ambList.number);

    refreshProcedureView();

    edited = false;
    makeEdited();
}

void ListPresenter::setfullCoverage(bool unfav)
{
    m_ambList.full_coverage = unfav;

    /*
    for (auto& m : m_ambList.procedures)
    {
        if (m.nzok)
        {
            m.price = MasterNZOK::instance().
                getPatientPrice
                (
                    m.code,
                    m_ambList.getAmbSheetDateMin(),
                    User::currentUser().doctor.specialty,
                    patient->isAdult(m.date),
                    unfav
                );

        }
    }
    */
    if (unfav)
    {
        m_ambList.charge = Charge::freed;
        view->taxCombo()->setIndex(2);
    }

    makeEdited();

   // refreshProcedureView();
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
