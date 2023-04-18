#include "ListPresenter.h"

#include "Database/DbPatient.h"
#include "Database/DbReferral.h"
#include "Database/DbAmbList.h"

#include "Model/Dental/NhifProcedures.h"
#include "Model/User.h"
#include "Model/Validators/AmbListValidator.h"

#include "Presenter/PatientDialogPresenter.h"
#include "Presenter/AllergiesDialogPresenter.h"
#include "Presenter/ProcedureDialogPresenter.h"
#include "Presenter/ProcedureEditorPresenter.h"
#include "Presenter/ReferralPresenter.h"
#include "Presenter/TabPresenter.h"

#include "View/ModalDialogBuilder.h"
#include "View/ModalDialogBuilder.h"
#include "View/Printer.h"

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

    if (m_ambList.rowid) return;

    //the list is NEW:
    if (User::practice().isUnfavourable() && patient->city.isUnfav()) {
        m_ambList.nhifData.isUnfavourable = true;
    }

    m_ambList.number = DbAmbList::getNewNumber(m_ambList.getDate(), m_ambList.isNhifSheet());
    m_ambList.lrn = FreeFn::getUuid();

}

void ListPresenter::statusChanged()
{
    m_checkModel = CheckModel(m_ambList.teeth.getSelectedTeethPtr(m_selectedIndexes));

    view->setCheckModel(m_checkModel);;
    
    for (auto& idx : m_selectedIndexes)
    {
        view->repaintTooth(
            ToothHintCreator::getToothHint(m_ambList.teeth[idx], patient->teethNotes[idx])
        );
    }

    if (m_selectedIndexes.size() == 1)
        surf_presenter.setTooth(m_ambList.teeth[m_selectedIndexes[0]]);

    makeEdited();
}

void ListPresenter::refreshPrices()
{
  //  double patientPrice(0);
    double nzokPrice(0);

    for (auto& m : m_ambList.procedures.list())
    {
      //  patientPrice = patientPrice + m.price;

        if (m.isNhif())
        {
            auto [p, nhif] = NhifProcedures::getPrices(m.code.nhifCode(), m_ambList.getDate(), patient->isAdult(m.date), User::doctor().specialty, m_ambList.nhifData.specification);
            nzokPrice = nzokPrice + nhif;
        }

    }

    view->refreshPriceLabel(nzokPrice);
}

void ListPresenter::setHisButtonToView()
{
    if (m_ambList.nrn.empty()) {
        
        view->setHisButtonText(
            IListView::HisButtonProperties
            {
                .hideSpinBox = false,
                .labelText = "Амб лист № :",
                .buttonText = "Изпрати към НЗИС",
                .hoverText = "Изпрати към НЗИС"
            }
        );

        return;
    }

    if (m_ambList.nrn.size())
    {
        view->setHisButtonText(
            IListView::HisButtonProperties
            {
                .hideSpinBox = true,
                .labelText = "НРН :",
                .buttonText = m_ambList.his_updated ? m_ambList.nrn : "Изпрати за корекция",
                .hoverText = m_ambList.his_updated ? "Анулирай" : "Изпрати за корекция"
            }
        );

        return;
    }


}

void ListPresenter::makeEdited()
{
    TabInstance::makeEdited();

    if (m_ambList.nrn.size()) {
        m_ambList.his_updated = false;
        setHisButtonToView();
    }
}


void ListPresenter::dynamicNhifConversion()
{
    if (m_ambList.isNhifSheet()) {

        bool practiceIsUnfav =
            User::practice().nzok_contract &&
            User::practice().nzok_contract->unfavourable
            ;

        view->setNhifData(m_ambList.nhifData, practiceIsUnfav);
    }
    else
    {
        view->hideNhifSheetData();
    }

}

bool ListPresenter::isValid()
{
    /*
    //check date inconsistencies
    if (m_ambList.procedures.empty() && m_ambList.referrals.empty()) {
        ModalDialogBuilder::showError("Листът трябва да съдържа поне една манипулация или направление!");
        return false;
    }
    */
    //check procedures and hyperdontic:

    for (auto& p : m_ambList.procedures)
    {
        if (p.hyperdontic && !m_ambList.teeth[p.tooth].hyperdontic)
        {
            ModalDialogBuilder::showError(
            "За да запишете манипулация на свръхброен зъб, отбележете го като такъв в статуса!"
            );
            return false;
        }
    }

    auto date = m_ambList.getDate();

    for (auto& p : m_ambList.procedures)
    {
        if (p.date.month != date.month || p.date.year != date.year)
        {
            ModalDialogBuilder::showError("Манипулациите трябва да са от един и същи месец!");
            return false;
        }

    }

    if (m_ambList.isNhifSheet() && !patient->PISHistory.has_value()) {
        ModalDialogBuilder::showMessage(
            "Не са заредени данни от ПИС. "
            "Листът ще бъде валидиран само "
            "спрямо съществуващите записи в локалната база данни");
    }


    AmbListValidator checker(m_ambList, *patient);

    if (checker.ambListIsValid()) return true;

    return 
        ModalDialogBuilder::askDialog(
            checker.getErrorMsg() + 
            "\nЖелаете ли да запазите листа въпреки това?" 
    );

}



void ListPresenter::chargeChanged(int index)
{
    m_ambList.nhifData.charge = static_cast<NhifCharge>(index);
    makeEdited();
}

TabName ListPresenter::getTabName()
{
    TabName n;


    n.header += m_ambList.isNew() ? "Нов амб.лист" :
        "Амб.лист ";
    
    n.header += m_ambList.nrn.size() ?
            m_ambList.nrn
            : 
            "№" + std::to_string(m_ambList.number);

   
    n.footer = patient->FirstName;
    n.footer += " ";
    n.footer += patient->LastName;

    n.nhif = m_ambList.isNhifSheet() || m_ambList.referrals.size();
    n.his = m_ambList.nrn.size();
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
            "Амбулаторен лист с такъв номер вече съществува. "
            "Сигурни ли сте че искате да дублирате номерацията?"))
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

    patient_info.setCurrent(true);

    view->setAmbListNum(m_ambList.number);

    setHisButtonToView();

    view->setDateTime(m_ambList.date);

    surf_presenter.setStatusControl(this);
    surf_presenter.setView(view->surfacePanel());
    view->surfacePanel()->setPresenter(&surf_presenter);

    auto hints = ToothHintCreator::getTeethHint(m_ambList.teeth);
    for (auto& hint : hints)
    {
        view->repaintTooth(hint);
    }

    view->setNotes(patient->teethNotes);
    
    view->setReferrals(m_ambList.referrals);

    refreshPrices();
    refreshProcedureView();
    dynamicNhifConversion();

    view->setSelectedTeeth(m_selectedIndexes);

    if (User::settings().getPisHistoryAuto &&
        User::hasNzokContract() &&
        !patient->PISHistory.has_value()) {
        requestPisActivities();
    }
   
}


void ListPresenter::setAmbDateTime(const std::string& datetime)
{
    m_ambList.date = datetime;
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

    for (auto idx : m_selectedIndexes) m_ambList.teeth[idx].setStatus(StatusType::caries, surface, state);

    statusChanged();
}

void ListPresenter::setObturation(int surface)
{
    bool state = m_checkModel.obturationStatus[surface] != CheckState::checked;

    for (auto idx : m_selectedIndexes) m_ambList.teeth[idx].setStatus(StatusType::obturation, surface, state);

    statusChanged();
}

void ListPresenter::setMainStatus(int code)
{
    bool state = m_checkModel.generalStatus[code] != CheckState::checked;

    m_ambList.teeth.setStatus(m_selectedIndexes, static_cast<StatusCode::StatusCode>(code), state);

    if (code == StatusCode::Temporary) {
        refreshPrices();
        refreshProcedureView(); //updates the teeth num
    }

    for (int i = 0; i < 32; i++){
        view->repaintTooth(ToothHintCreator::getToothHint(m_ambList.teeth[i], patient->teethNotes[i]));
    }

    statusChanged();
}

void ListPresenter::setMobility(int degree)
{
    bool state = m_checkModel.mobilityStatus[degree] != CheckState::checked;

    for (auto idx : m_selectedIndexes) m_ambList.teeth[idx].setStatus(StatusType::mobility, degree, state);

    statusChanged();

}

void ListPresenter::setOther(int code)
{
    auto DO = [](Tooth& t) mutable
    {
        t.removeStatus(StatusCode::Obturation);
        t.setStatus(StatusType::obturation, Surface::Distal);
        t.setStatus(StatusType::obturation, Surface::Occlusal);
    };

    auto MO = [](Tooth& t) mutable
    {
        t.removeStatus(StatusCode::Obturation);
        t.setStatus(StatusType::obturation, Surface::Medial);
        t.setStatus(StatusType::obturation, Surface::Occlusal);
    };

    auto MOD = [](Tooth& t)
    {
        t.removeStatus(StatusCode::Obturation);
        t.setStatus(StatusType::obturation, Surface::Medial);
        t.setStatus(StatusType::obturation, Surface::Distal);
        t.setStatus(StatusType::obturation, Surface::Occlusal);
    };

    auto& teeth = m_ambList.teeth;
    for (auto idx : m_selectedIndexes)
    {

        auto& tooth = m_ambList.teeth[idx];

        switch (code)
        {
            case OtherInputs::DO: DO(tooth); break;
            case OtherInputs::MO: MO(tooth); break;
            case OtherInputs::MOD: MOD(tooth); break;
            case OtherInputs::removeC: tooth.removeStatus(StatusType::caries); break;
            case OtherInputs::removeO: tooth.removeStatus(StatusType::obturation); break;
            case OtherInputs::removeBridge:
                teeth.removeBridgeOrSplint(m_selectedIndexes);
                break;
            case OtherInputs::removeAll:
                teeth.removeEveryStatus(m_selectedIndexes);
                break;
        }
    }

    for (int i = 0; i < 32; i++)
    {
        view->repaintTooth(ToothHintCreator::getToothHint(m_ambList.teeth[i], patient->teethNotes[i]));
    }

    statusChanged();
}


void ListPresenter::setSelectedTeeth(const std::vector<int>& SelectedIndexes)
{
    m_selectedIndexes = SelectedIndexes;

    m_checkModel = CheckModel(m_ambList.teeth.getSelectedTeethPtr(SelectedIndexes));

    view->setCheckModel(m_checkModel);

    if(m_selectedIndexes.size() == 1){
        surf_presenter.setTooth(m_ambList.teeth[m_selectedIndexes[0]]);
        view->hideSurfacePanel(false);
    }
    else {
        view->hideSurfacePanel(true);
    }

    view->hideControlPanel(m_selectedIndexes.empty());

}


int ListPresenter::generateAmbListNumber()
{
    int newNumber = m_ambList.number;

    auto ambSheetDate = m_ambList.getDate();

    if (m_ambList.isNew()) {
        newNumber = DbAmbList::getNewNumber(ambSheetDate, m_ambList.isNhifSheet());
    }

    return newNumber;
}

void ListPresenter::requestPisActivities()
{
    dentalActService.sendRequest(
        patient->type, 
        patient->id,
        [&](auto procedures, auto payment) { 
            setPISActivities(procedures, payment);
        },
        m_openHistoryDialogOnReply
    );

}

void ListPresenter::setPISActivities(
    const std::optional<std::vector<Procedure>>& pisProcedures,
    const std::vector<std::string>& payment_status
)
{

    if (!pisProcedures.has_value()) {
        m_openHistoryDialogOnReply = false;
        return;
    }

    patient->PISHistory = pisProcedures;
    patient->pis_paymentStatus = payment_status;

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
        ModalDialogBuilder::showMessage("В ПИС не са намерени данни за този пациент");
        return;
    }

    bool applyToStatus = ModalDialogBuilder::pisHistoryDialog(history, patient->pis_paymentStatus);

    if (!applyToStatus) return;
    
    for (auto it = history.rbegin(); it != history.rend(); ++it)
    it->applyPISProcedure(m_ambList.teeth);

    if (isCurrent()) {

        for (auto& t : m_ambList.teeth)
            view->repaintTooth(ToothHintCreator::getToothHint(t, patient->teethNotes[t.index]));
    }
   

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
        view->repaintTooth(ToothHintCreator::getToothHint(m_ambList.teeth[i], patient->teethNotes[i]));
    }

    

    statusChanged();
}

void ListPresenter::openDetails()
{
    if (m_selectedIndexes.size() == 1) openDetails(m_selectedIndexes[0]);
}



void ListPresenter::refreshProcedureView()
{
    if (view == nullptr) return;

    auto& mList = m_ambList.procedures;

    m_ambList.procedures.refreshTeethTemporary(m_ambList.teeth);

    view->setProcedures(m_ambList.procedures.list());
}

void ListPresenter::addProcedure()
{
    ProcedureDialogPresenter p
    {
        m_ambList,
        m_ambList.teeth.getSelectedTeethPtr(m_selectedIndexes),
        patient->turns18At(),
        patient->canBePregnant(m_ambList.getDate())
    };

    auto procedures = p.openDialog();

    if (procedures.empty()) return;

    m_ambList.procedures.addProcedures(procedures);

    if (view == nullptr) return;

    patient_info.setDate(m_ambList.getDate());

    dynamicNhifConversion();

    refreshPrices();
    refreshProcedureView();

    makeEdited();

}

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

    patient_info.setDate(m_ambList.getDate());
    refreshPrices();
    refreshProcedureView();

    dynamicNhifConversion();


    makeEdited();
}

void ListPresenter::moveProcedure(int from, int to)
{
    if(!m_ambList.procedures.moveProcedure(from, to)) return;

    makeEdited();

    view->setProcedures(m_ambList.procedures.list());
}

void ListPresenter::addReferral(ReferralType type)
{

    if (!User::hasNzokContract()) {
        ModalDialogBuilder::showMessage(
            "За да създадете направление, трябва да попълните данните" 
            "\nна договора с НЗОК и кодът на специалността от насторйки!"
        );

        return;
    }

    if (type != ReferralType::MDD4) {
        for (auto& r : m_ambList.referrals)
        {
            if (r.type == type) {
                ModalDialogBuilder::showMessage(
                    "Позволено е максимум по едно направление от тип бл.3, бл.3А и 119 МЗ"
                );
                return;
            }
        }
    }
    else
    {
        int mddCounter = 0;

        for (auto& r : m_ambList.referrals)
        {
            if (r.type == ReferralType::MDD4) mddCounter++;
        }

        if (mddCounter >= 2) {
            ModalDialogBuilder::showMessage(
                "Позволени са максимум две направления за медико-диагностична дейност"
            );
            return;
        }
    }

    ReferralPresenter p(m_ambList, type);

    auto result = p.openDialog();

    if (!result) return;

    m_ambList.referrals.push_back(*result);

    view->setReferrals(m_ambList.referrals);

    dynamicNhifConversion();

    makeEdited();

}

void ListPresenter::editReferral(int index)
{
    ReferralPresenter p(m_ambList.referrals[index]);

    auto result = p.openDialog();

    if (!result) return;

    m_ambList.referrals[index] = *result;

    view->setReferrals(m_ambList.referrals);

    makeEdited();
}

void ListPresenter::removeReferral(int index)
{
    auto& r = m_ambList.referrals[index];

    if (!r.isNrnType() || !r.isSentToHIS())
    {
        auto& rList = m_ambList.referrals;

        rList.erase(rList.begin() + index);

        view->setReferrals(rList);

        dynamicNhifConversion();

        makeEdited();

        return;
    }
    
    //query for his invalidation, then removal!

}

void ListPresenter::printReferral(int index)
{
    if (m_ambList.referrals[index].isNrnType()) return;

    Print::referral(m_ambList.referrals[index], *patient.get(), m_ambList.number);
}

void ListPresenter::sendReferralToHis(int index)
{
    auto& ref = m_ambList.referrals[index];

    if (!ref.isNrnType()) return;

    if (m_ambList.nrn.empty()) {
        ModalDialogBuilder::showMessage("За да издадете направление, първо изпратете амбулаторния лист в НЗИС");
        return;
    }

    if (ref.nrn.empty())
    {
        eReferralIssueService.sendRequest(
            m_ambList.nrn,
            *patient.get(),
            m_ambList.referrals[index],
            [=](const std::string& nrn) {

                m_ambList.referrals[index].nrn = nrn;
                DbAmbList::update(m_ambList);
                if (isCurrent()) {
                    view->setReferrals(m_ambList.referrals);
                }

            }
        );

        return;
    }

    eReferralCancelService.sendRequest(
        ref.nrn,
        [=](bool success) {
            if (!success) return;

            m_ambList.referrals[index].nrn.clear();
            DbAmbList::update(m_ambList);
            if (isCurrent()) {
                view->setReferrals(m_ambList.referrals);
            }
        }
    );


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

    tabPresenter->openInvoice(patient->rowid, selectedProcedures.value());
}

void ListPresenter::createPerioMeasurment()
{
    tabPresenter->openPerio(*this->patient.get());
}

void ListPresenter::createPrescription()
{
    tabPresenter->openPerscription(*this->patient.get());
}

void ListPresenter::hisButtonPressed()
{


    if (m_ambList.nrn.empty()) {

        if (!save()) return;

        eDentalOpenService.sendRequest(
            m_ambList,
            *patient,
            [this](auto& nrn, auto& procedureIndex) {
                if (nrn.empty()) {
                    return;
                }
                
                m_ambList.nrn = nrn;

                for (int i = 0; i < procedureIndex.size(); i++) {
                    m_ambList.procedures[i].his_index = procedureIndex[i];
                }

                DbAmbList::update(m_ambList);
                
                refreshTabName();

                if (isCurrent())
                {
                    setHisButtonToView();
                }


            }
        );

        return;
    }

    if (!m_ambList.his_updated)
    {
        eDentalAugmentService.sendRequest(m_ambList, *patient,
            [this](auto& procedureIdx)
            {
                m_ambList.his_updated = true;

                m_ambList.procedures.clearRemovedProcedures();

                for (auto& [sequence, hisIdx] : procedureIdx)
                {
                    m_ambList.procedures[sequence].his_index = hisIdx;
                }

                DbAmbList::update(m_ambList);

                refreshTabName();

                ModalDialogBuilder::showMessage("Денталният преглед е коригиран успешно");

                if (isCurrent())
                {
                    setHisButtonToView();
                }
            }
        );

        return;
    }

    if (m_ambList.nrn.size()) {
        eDentalCancelService.sendRequest(m_ambList.nrn,
            [this](bool success) {

                if (!success) return;

                m_ambList.nrn.clear();

                m_ambList.procedures.clearRemovedProcedures();

                for (auto& p : m_ambList.procedures) p.his_index = 0;

                DbAmbList::update(m_ambList);
                
                refreshTabName();

                ModalDialogBuilder::showMessage("Денталният преглед е анулиран успешно");

                if (isCurrent())
                {
                    setHisButtonToView();
                }
        });

        return;
    }


}

void ListPresenter::getStatusPressed()
{
    eDentalGetService.sendRequest(
        *patient.get(),
        [this](const ToothContainer& teeth, const ProcedureContainer& p) {
            m_ambList.teeth = teeth;
            makeEdited();
            if (isCurrent()) {
                for (int i = 0; i < 32; i++)
                {
                    view->repaintTooth(ToothHintCreator::getToothHint(m_ambList.teeth[i], patient->teethNotes[i]));
                }
            }
        }
    );
}


ListPresenter::~ListPresenter()
{
    if (view) view->setPresenter(nullptr);
}
