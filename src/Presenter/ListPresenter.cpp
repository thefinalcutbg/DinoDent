#include "ListPresenter.h"

#include "Database/DbPatient.h"
#include "Database/DbReferral.h"
#include "Database/DbAmbList.h"
#include "Database/DbProcedure.h"
#include "Model/Dental/NhifProcedures.h"
#include "Model/User.h"
#include "Model/Validators/AmbListValidator.h"

#include "Presenter/PatientDialogPresenter.h"
#include "Presenter/ProcedureDialogPresenter.h"
#include "Presenter/ProcedureEditorPresenter.h"
#include "Presenter/ReferralPresenter.h"
#include "Presenter/TabPresenter.h"
#include "Presenter/ProcedureHistoryPresenter.h"

#include "View/ModalDialogBuilder.h"
#include "View/ModalDialogBuilder.h"
#include "View/Printer.h"
#include "Network/PKCS11.h"

#include "View/Graphics/PaintHint.h"

#include <thread>
#include <chrono>
#include <qdebug.h>
ListPresenter::ListPresenter(ITabView* tabView, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient, long long rowId)
    :
    TabInstance(tabView, TabType::AmbList, patient),
    view(tabView->listView()),
    tabPresenter(tabPresenter),
    m_ambList(rowId ? DbAmbList::getListData(rowId) : DbAmbList::getNewAmbSheet(patient->rowid)),
    patient_info(tabView->listView()->tileInfo(), patient)
{
    surf_presenter.setStatusControl(this);

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
    m_dsnCheckModel = CheckModel(m_ambList.teeth.getSelectedDsnPtr(m_selectedIndexes));

    view->setCheckModel(m_checkModel, m_dsnCheckModel);

    for (auto& idx : m_selectedIndexes)
    {
        view->repaintTooth(
            ToothPaintHint(m_ambList.teeth[idx], patient->teethNotes[idx])
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
            auto [p, nhif] = NhifProcedures::getPrices(m.code.oldCode(), m_ambList.getDate(), patient->isAdult(m.date), User::doctor().specialty, m_ambList.nhifData.specification);
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
        if(isCurrent()) setHisButtonToView();
    }
}


void ListPresenter::dynamicNhifConversion()
{
    if (m_ambList.isNhifSheet()) {

        bool practiceIsUnfav =
            User::practice().nhif_contract &&
            User::practice().nhif_contract->unfavourable
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

    for (auto& p : m_ambList.procedures)
    {
        if (p.tooth_idx.supernumeral && !m_ambList.teeth[p.tooth_idx.index].dsn)
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
        if (p.date < date) {
            ModalDialogBuilder::showError("Датата на манипулациите не трябва да е по-малка от тази на амбулаторния лист");
            return false;
        }

        if (p.date.month != date.month || p.date.year != date.year)
        {
            ModalDialogBuilder::showError("Манипулациите и амбулаторният лист трябва да са от един и същи месец!");
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

    if (
        m_ambList.nrn.empty() && 
        DbAmbList::suchNumberExists(d.year, m_ambList.number, m_ambList.rowid) &&
       !ModalDialogBuilder::askDialog(
            "Амбулаторен лист с такъв номер вече съществува. "
            "Сигурни ли сте че искате да дублирате номерацията?"
        )
    )
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

    for (int i = 0; i < 32; i++)
    {
        view->repaintTooth(ToothPaintHint(m_ambList.teeth[i], patient->teethNotes[i]));
    }

    view->setNotes(patient->teethNotes);
    
    view->setReferrals(m_ambList.referrals);

    refreshPrices();
    refreshProcedureView();
    dynamicNhifConversion();

    view->setSelectedTeeth(m_selectedIndexes);

    if (firstFocus)
    {
        if (User::settings().getPisHistoryAuto && User::hasNzokContract()) {

            auto callback = [&](const std::optional<std::vector<Procedure>>& result) {

                if (!result) return;

                auto& procedures = result.value();

                patient->PISHistory = procedures;
            };

            dentalActService.sendRequest(patient->type, patient->id, false, callback);
        }

        if (User::settings().getHisHistoryAuto) {
            
            auto callback = [&](const std::optional<std::vector<Procedure>>& result) {

                if (!result) return;

                auto& procedures = result.value();

                patient->HISHistory = procedures;
            };

            eDentalGetProcedures.sendRequest(*patient, false, callback);

        }

        firstFocus = false;
    }
}


void ListPresenter::setAmbDateTime(const std::string& datetime)
{
    m_ambList.date = datetime;
    makeEdited();
}

void ListPresenter::ambNumChanged(long long value)
{
    m_ambList.number = value;
    edited = false;
    makeEdited();
}

void ListPresenter::checkPention()
{
    nssiService.sendRequest(*patient, [](const std::optional<Pention>& p) {}, true);
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
            case OtherInputs::removeDsn:
                teeth.setStatus(m_selectedIndexes, StatusType::general, StatusCode::Dsn, false, false);
                break;
        }
    }

    for (int i = 0; i < 32; i++)
    {
        view->repaintTooth(ToothPaintHint{ m_ambList.teeth[i], patient->teethNotes[i] });
    }

    statusChanged();
}

void ListPresenter::setToothStatus(StatusType t, int code)
{
    bool state{ false };

    switch (t)
    {
        case StatusType::general: state = m_checkModel.generalStatus[code] != CheckState::checked; break;
        case StatusType::obturation: state = m_checkModel.obturationStatus[code] != CheckState::checked; break;
        case StatusType::caries: state = m_checkModel.cariesStatus[code] != CheckState::checked; break;
        case StatusType::mobility: state = m_checkModel.mobilityStatus[code] != CheckState::checked; break;
    }

    m_ambList.teeth.setStatus(m_selectedIndexes, t, static_cast<StatusCode::StatusCode>(code), state, false);

    if (t == StatusType::general)
    {
        if (code == StatusCode::Temporary) {
            refreshProcedureView(); //updates the teeth num
        }

        for (int i = 0; i < 32; i++) {
            view->repaintTooth(ToothPaintHint(m_ambList.teeth[i], patient->teethNotes[i]));
        }
    }

    statusChanged();
}

void ListPresenter::setDsnStatus(StatusType t, int code)
{

    bool state{ false };

    switch (t)
    {
        case StatusType::general: state = m_dsnCheckModel.generalStatus[code] != CheckState::checked; break;
        case StatusType::obturation: state = m_dsnCheckModel.obturationStatus[code] != CheckState::checked; break;
        case StatusType::caries: state = m_dsnCheckModel.cariesStatus[code] != CheckState::checked; break;
        case StatusType::mobility: state = m_dsnCheckModel.mobilityStatus[code] != CheckState::checked; break;
    }

    m_ambList.teeth.setStatus(m_selectedIndexes, t, static_cast<StatusCode::StatusCode>(code), state, true);

    statusChanged();
}

void ListPresenter::setSelectedTeeth(const std::vector<int>& SelectedIndexes)
{
    m_selectedIndexes = SelectedIndexes;

    m_checkModel = CheckModel(m_ambList.teeth.getSelectedTeethPtr(SelectedIndexes));
    m_dsnCheckModel = CheckModel(m_ambList.teeth.getSelectedDsnPtr(SelectedIndexes));

    view->setCheckModel(m_checkModel, m_dsnCheckModel);

    if(m_selectedIndexes.size() == 1){
        surf_presenter.setTooth(m_ambList.teeth[m_selectedIndexes[0]]);
        view->hideSurfacePanel(false);
    }
    else {
        view->hideSurfacePanel(true);
    }

    view->hideControlPanel(m_selectedIndexes.empty());

}

void ListPresenter::historyRequested()
{
    if (dentalActService.awaitingReply() ||
        eDentalGetProcedures.awaitingReply())
    {
        ModalDialogBuilder::showMessage("Моля изчакайте, очаква се отговор от сървъра");
        return;
    }

    ProcedureHistoryPresenter p(*patient.get());

    p.openDialog();

    auto result = p.result();;

    if (result.pis_history) patient->PISHistory = result.pis_history;
    if (result.his_history) patient->HISHistory = result.his_history;

    if (!result.applyPis && !result.applyCurrentStatus) return;

    if (result.applyPis) {
        for (auto& p : *result.pis_history)
        {
            p.applyPISProcedure(m_ambList.teeth);
        }
    }

    if (result.applyCurrentStatus) {

        m_ambList.teeth.copyFromOther(result.current_status.value());
    }

    for (int i = 0; i < 32; i++) {
        view->repaintTooth(ToothPaintHint(m_ambList.teeth[i], patient->teethNotes[i]));
    }

    statusChanged();
}


std::vector<Procedure> ListPresenter::getToothHistory(int toothIdx)
{
    std::vector<Procedure> result = DbProcedure::getToothProcedures(patient->rowid, toothIdx);

    if (patient->HISHistory) {
        for (auto& p : patient->HISHistory.value())
        {
            if (p.tooth_idx.index == toothIdx) result.push_back(p);
        }
    }

    if (patient->PISHistory)
    {
        for (auto& p : patient->PISHistory.value())
        {
            if (p.tooth_idx.index == toothIdx) result.push_back(p);
        }
    }

    std::sort(result.begin(), result.end(), [](const Procedure& a, const Procedure& b) -> bool
        {
            return a.date > b.date;
        }
    );

    return result;
    
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



#include "Presenter/DetailedStatusPresenter.h"

void ListPresenter::openDetails(int toothIdx)
{
    DetailedStatusPresenter d(m_ambList.teeth[toothIdx], patient->rowid, getToothHistory(toothIdx));

    auto result = d.open();

    if (!result.has_value()) return;
    
    m_ambList.teeth.setToothDetails(result.value());

    patient->teethNotes[toothIdx] = d.getNote();

    view->setNotes(patient->teethNotes);
   
    for (int i = 0; i < 32; i++)
    {
        view->repaintTooth(ToothPaintHint(m_ambList.teeth[i], patient->teethNotes[i]));
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

    if (type == ReferralType::Form3 || type == ReferralType::Form3A) {
        for (auto& r : m_ambList.referrals)
        {
            if (r.type == ReferralType::Form3 || type == ReferralType::Form3A) {
                ModalDialogBuilder::showMessage(
                    "Позволено е максимум едно направление от тип бл.3 или бл.3А"
                );
                return;
            }
        }
    }
    else if(type == ReferralType::MDD4)
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
    else
    {
        for (auto& r : m_ambList.referrals)
        {
            if (r.type == ReferralType::MH119) {

                ModalDialogBuilder::showMessage(
                    "Позволено е максимум по едно направление от тип 119 МЗ"
                );
                return;
            }
        }

    }

    ReferralPresenter p(m_ambList, type);

    auto result = p.openDialog();

    if (!result) return;

    m_ambList.referrals.push_back(*result);

    view->setReferrals(m_ambList.referrals);

    dynamicNhifConversion();
   
    if (!m_ambList.isNew()) {
        DbReferral::saveReferrals(m_ambList.referrals, m_ambList.rowid);
    }

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

                DbReferral::saveReferrals(m_ambList.referrals, m_ambList.rowid);

                if (isCurrent()) {
                    view->setReferrals(m_ambList.referrals);
                }

            }
        );

        return;
    }
}


void ListPresenter::removeReferral(int index)
{
    auto& r = m_ambList.referrals[index];

    if (!r.isNrnType() || !r.isSentToHIS())
    {
        auto& rList = m_ambList.referrals;

        rList.erase(rList.begin() + index);

        if(isCurrent()) view->setReferrals(rList);

        dynamicNhifConversion();

        refreshTabName();

        TabInstance::makeEdited(); //no need for his augmentation

        return;
    }


    if (r.isSentToHIS()) {

        eReferralCancelService.sendRequest(r.nrn,
            [&](bool success) {

                if (!success) return;

                m_ambList.referrals.erase(m_ambList.referrals.begin() + index);

                DbReferral::saveReferrals(m_ambList.referrals, m_ambList.rowid);

                ModalDialogBuilder::showMessage("Направлението е анулирано успешно!");

                dynamicNhifConversion();

                if(isCurrent()) view->setReferrals(m_ambList.referrals);

                refreshTabName();

               
            }
        );

    }

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
            [&](auto& nrn, auto& procedureIndex) {
                if (nrn.empty()) {
                    return;
                }
                
                m_ambList.nrn = nrn;

                for (int i = 0; i < procedureIndex.size(); i++) {
                    m_ambList.procedures[i].his_index = procedureIndex[i];
                }

                m_ambList.his_updated = true;

                DbAmbList::update(m_ambList);

                refreshTabName();

                if (isCurrent())
                {
                    setHisButtonToView();
                    view->setProcedures(m_ambList.procedures.list());
                }

                ModalDialogBuilder::showMessage("Денталният преглед е изпратен към НЗИС успешно");


            }
        );

        return;
    }

    if (!m_ambList.his_updated)
    {
        if (!isValid()) return;

        eDentalAugmentService.sendRequest(m_ambList, *patient,
            [&](auto& procedureIdx)
            {
                m_ambList.his_updated = true;

                m_ambList.procedures.clearRemovedProcedures();

                for (auto& [sequence, hisIdx] : procedureIdx)
                {
                    m_ambList.procedures[sequence].his_index = hisIdx;
                }

                DbAmbList::update(m_ambList);

                edited = false;

                refreshTabName();

                if (isCurrent())
                {
                    setHisButtonToView();
                    view->setProcedures(m_ambList.procedures.list());
                }

                ModalDialogBuilder::showMessage("Денталният преглед е коригиран успешно");


            }
        );

        return;
    }

    if (m_ambList.nrn.size()) {
        eDentalCancelService.sendRequest(m_ambList.nrn,
            [&](bool success) {

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
                    view->setProcedures(m_ambList.procedures.list());
                }
        });

        return;
    }


}


ListPresenter::~ListPresenter()
{
    if (view) view->setPresenter(nullptr);
}
