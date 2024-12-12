#include "ListPresenter.h"

#include "Database/DbReferral.h"
#include "Database/DbAmbList.h"
#include "Database/DbProcedure.h"
#include "Database/DbMedicalNotice.h"
#include "Model/User.h"
#include "Model/Validators/AmbListValidator.h"

#include "Presenter/ProcedureDialogPresenter.h"
#include "Presenter/ProcedureEditorPresenter.h"
#include "Presenter/ReferralPresenter.h"
#include "Presenter/TabPresenter.h"
#include "Presenter/ProcedureHistoryPresenter.h"
#include "Presenter/DetailedStatusPresenter.h"
#include "Presenter/FiscalReceiptPresenter.h"
#include "Model/FreeFunctions.h"

#include "View/ModalDialogBuilder.h"
#include "View/Printer.h"
#include "View/Graphics/PaintHint.h"

ListPresenter::ListPresenter(ITabView* tabView, std::shared_ptr<Patient> patient, long long rowId)
    :
    TabInstance(tabView, TabType::AmbList, patient),
    patient_info(tabView->listView()->tileInfo(), patient),
    view(tabView->listView()),
    m_ambList(rowId ? DbAmbList::getListData(rowId) : DbAmbList::getNewAmbSheet(patient->rowid))
{
    surf_presenter.setStatusControl(this);

    if (m_ambList.rowid) return;

    //the list is NEW:
    if (patient->city.isUnfav() && 
        User::practice().isUnfavourable()
    ) {
        m_ambList.nhifData.isUnfavourable = true;
    }
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

void ListPresenter::setHisButtonToView()
{
    if (m_ambList.nrn.empty()) {
        
        view->setHisButtonText(
            IListView::HisButtonProperties
            {
                .hideSpinBox = false,
                .buttonText = "Изпрати към НЗИС",
                .hoverText = "Изпрати към НЗИС"
            }
        );

        return;
    }

    if (m_ambList.nrn.size())
    {
        IListView::HisButtonProperties prop
        {
            .hideSpinBox = true,
            .buttonText = m_ambList.his_updated ? m_ambList.nrn : "Изпрати за корекция",
            .hoverText = m_ambList.his_updated ? "Анулирай" : "Изпрати за корекция"
        };

        view->setHisButtonText(prop);

        return;
    }


}

void ListPresenter::makeEdited()
{
    if (m_ambList.nrn.size()) {
        m_ambList.his_updated = false;
        if (isCurrent()) setHisButtonToView();
    }

    TabInstance::makeEdited();


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
    bool nonExamProcedureFound = false;

    for (auto& p : m_ambList.procedures)
    {

        bool isFullExam = p.code.type() == ProcedureType::FullExam;

        if (!isFullExam) {
            nonExamProcedureFound = true;
        }

        if (nonExamProcedureFound && isFullExam)
        {
            ModalDialogBuilder::showError(
                "С оглед на правилната хронология на пациентското досие в НЗИС, обстойният преглед трябва да бъде винаги първа по ред процедура. Преместете го (при необходимост - редактирайте датата му)."
            );

            return false;
        }
    }


    for (auto& p : m_ambList.procedures)
    {
        auto& idx = p.getToothIndex();

        if (idx.supernumeral && !m_ambList.teeth[idx.index][Dental::HasSupernumeral])
        {
            ModalDialogBuilder::showError(
            "За да запишете процерура на свръхброен зъб, отбележете го като такъв в статуса!"
            );
            return false;
        }

        if (p.code.getScope() == ProcedureScope::SingleTooth
            && m_ambList.teeth.at(p.getToothIndex()).noData())
        {

            std::string errMsg = "За процедура ";
            errMsg += p.code.code();
            errMsg += " въвеждането на статус на зъб ";
            errMsg += idx.getNhifNumenclature();
            errMsg += " е задължително";

            ModalDialogBuilder::showError(errMsg);
            return false;
        }
    }

    auto date = m_ambList.getDate();

    for (auto& p : m_ambList.procedures)
    {
        if (p.date < date) {
            ModalDialogBuilder::showError("Датата на процедурите не трябва да е по-малка от тази на амбулаторния лист");
            return false;
        }

        if (p.date.month != date.month || p.date.year != date.year)
        {
            ModalDialogBuilder::showError("Процедурите и амбулаторният лист трябва да са от един и същи месец!");
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


TabName ListPresenter::getTabName()
{
    TabName n;

    if (m_ambList.isNew()) {
        n.header += "Нов амб. лист ";
    }
    else if (m_ambList.nrn.size()) {
        n.header += "Амб. лист ";
        n.header += m_ambList.nrn;
    }
    else if (m_ambList.number){
        n.header += "Амб. лист №";
        n.header += std::to_string(m_ambList.number);
    }
    else {
        n.header += "Амбулаторен лист ";
    }

    n.footer = patient->FirstName;
    n.footer += " ";
    n.footer += patient->LastName;

    if (m_ambList.isNhifSheet()) {
        n.footer_icon = CommonIcon::NHIF;
    }
    
    if (m_ambList.nrn.size()) {
        n.header_icon = m_ambList.his_updated ? CommonIcon::HIS : CommonIcon::HISGRAY;
    }

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
    if (!save()) return;

    Print::ambList(m_ambList, *patient);
}

void ListPresenter::setDataToView()
{
    view->setPresenter(this);
    
    patient_info.setDate(m_ambList.getDate());

    patient_info.setCurrent(true);

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
    
    view->setAdditionalDocuments(m_ambList.referrals, m_ambList.medical_notices);

    refreshProcedureView();
    dynamicNhifConversion();

    view->setSelectedTeeth(m_selectedIndexes);

    if (firstFocus)
    {
        bool querySent = true; //prevents multiple PKCS11 prompts

        if (User::settings().getPisHistoryAuto && User::hasNhifContract()) {

            auto callback = [&](const std::optional<std::vector<Procedure>>& result) {

                if (!result) return;

                auto& procedures = result.value();

                patient->PISHistory = procedures;
            };

            querySent = dentalActService.sendRequest(*patient, false, callback);
        }

        if (!querySent) {
            firstFocus = false;
            return;
        }

        if (User::settings().getHisHistoryAuto) {
            
            auto callback = [&](const std::optional<std::vector<Procedure>>& result, const ToothContainer& teeth) {

                if (!result) return;
 
                auto& procedures = result.value();

                patient->HISHistory = procedures;
  
                if (!m_ambList.isNew()) return;

                m_ambList.teeth.copyOnlyOnUnknown(teeth);

                for (int i = 0; i < 32; i++)
                {
                    view->repaintTooth(ToothPaintHint{ m_ambList.teeth[i], patient->teethNotes[i] });
                }

                
            };

            eDentalGetStatusAndProceduresService.sendRequest(*patient, false, callback);

        }

        firstFocus = false;
    }
}


void ListPresenter::setAmbDateTime(const std::string& datetime)
{
    m_ambList.date = datetime;

    if (!User::hasNhifContract()) {

        auto pDate = Date(datetime);

        for (auto& p : m_ambList.procedures) {
            p.date = pDate;
        }

        view->setProcedures(m_ambList.procedures.list());
    }

    makeEdited();
}

void ListPresenter::checkPention()
{
    nssiService.sendRequest(*patient);
}

void ListPresenter::addFinancialReceipt()
{
    if (isNew()) {
        ModalDialogBuilder::showMessage(
            "Запазете амбулаторния лист преди да пуснете опис на касов бон" 
        );
        return;
    }

    FiscalReceiptPresenter p(rowID());
    p.openDialog();
}


void ListPresenter::setOther(int code)
{
    auto DO = [](Tooth& t) mutable
    {
        t.setStatus(Dental::Restoration, false);
        t.setSurface(Dental::Restoration, Dental::Distal, true);
        t.setSurface(Dental::Restoration, Dental::Occlusal, true);
    };

    auto MO = [](Tooth& t) mutable
    {
        t.setStatus(Dental::Restoration, false);
        t.setSurface(Dental::Restoration, Dental::Medial, true);
        t.setSurface(Dental::Restoration, Dental::Occlusal, true);
    };

    auto MOD = [](Tooth& t)
    {
        t.setStatus(Dental::Restoration, false);
        t.setSurface(Dental::Restoration, Dental::Distal, true);
        t.setSurface(Dental::Restoration, Dental::Occlusal, true);
        t.setSurface(Dental::Restoration, Dental::Medial, true);
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
            case OtherInputs::removeC: tooth.setStatus(Dental::Caries, false); break;
            case OtherInputs::removeO: tooth.setStatus(Dental::Restoration, false); break;
            case OtherInputs::removeBridge:
                teeth.removeBridgeOrSplint({ idx });
                break;
            case OtherInputs::removeAll:
                teeth.removeEveryStatus({ idx });
                break;
            case OtherInputs::removeDsn:
                tooth.setStatus(Dental::HasSupernumeral, false);
                break;
        }
    }

    for (int i = 0; i < 32; i++)
    {
        view->repaintTooth(ToothPaintHint{ m_ambList.teeth[i], patient->teethNotes[i] });
    }

    statusChanged();
}

void ListPresenter::setToothStatus(Dental::StatusType t, int code, bool supernumeral)
{
    //show multiple teeth warning message
    if (
        m_selectedIndexes.size() == 1 &&
        t == Dental::StatusType::General &&
        (code == Dental::Bridge || code == Dental::Splint) &&
        m_checkModel.generalStatus[code] == CheckState::unchecked
        )
    {
        ModalDialogBuilder::showMessage("За да добавите този статус изберете няколко зъба едновременно");
        return;
    }

    bool state{ false };

    auto& checkModel = supernumeral ? m_dsnCheckModel : m_checkModel;

    switch (t)
    {
        case Dental::StatusType::General: state = checkModel.generalStatus[code] != CheckState::checked; break;
        case Dental::StatusType::Restoration: state = checkModel.restorationStatus[code] != CheckState::checked; break;
        case Dental::StatusType::Caries: state = checkModel.cariesStatus[code] != CheckState::checked; break;
        case Dental::StatusType::Mobility: state = checkModel.mobilityStatus[code] != CheckState::checked; break;
    }

    m_ambList.teeth.setStatus(m_selectedIndexes, t, code, state, supernumeral);

    if (t == Dental::StatusType::General)
    {
        if (code == Dental::Temporary) {
            refreshProcedureView(); //updates the teeth num
        }

        for (int i = 0; i < 32; i++) {
            view->repaintTooth(ToothPaintHint(m_ambList.teeth[i], patient->teethNotes[i]));
        }
    }

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
        eDentalGetStatusAndProceduresService.awaitingReply()
    )
    {
        ModalDialogBuilder::showMessage("Моля изчакайте, очаква се отговор от сървъра");
        return;
    }

    ProcedureHistoryPresenter p(*patient.get());

    p.openDialog();

    auto result = p.result();;

    if (result.pis_history) patient->PISHistory = result.pis_history;
    if (result.his_history) patient->HISHistory = result.his_history;

    if (!result.applyPis && !result.statusToBeApplied) return;

    if (result.applyPis) {

        auto& procedures = *result.pis_history;

        for (int i = procedures.size()-1; i != -1; i--)
        {
            procedures[i].applyPISProcedure(m_ambList.teeth);
        }
    }

    if (result.statusToBeApplied) {

        m_ambList.teeth.copyFromHis(*result.statusToBeApplied);
    }

    for (int i = 0; i < 32; i++) {
        view->repaintTooth(ToothPaintHint(m_ambList.teeth[i], patient->teethNotes[i]));
    }

    statusChanged();
}

void ListPresenter::openDetails(int toothIdx)
{
    if (toothIdx < 0 || toothIdx > 31) return;

    std::vector<Procedure> history = DbProcedure::getToothProcedures(patient->rowid, toothIdx);

    if (patient->HISHistory) {
        for (auto& p : patient->HISHistory.value())
        {
            if (p.getToothIndex().index == toothIdx) history.push_back(p);
        }
    }

    if (patient->PISHistory)
    {
        for (auto& p : patient->PISHistory.value())
        {
            if (p.getToothIndex().index == toothIdx) history.push_back(p);
        }
    }

    std::sort(history.begin(), history.end(), [](const Procedure& a, const Procedure& b) -> bool
        {
            return a.date < b.date;
        }
    );

    DetailedStatusPresenter d(toothIdx, patient->rowid, history);

    d.open();

    patient->teethNotes[toothIdx] = d.getNote();

    view->setNotes(patient->teethNotes);
  
    view->repaintTooth(ToothPaintHint(m_ambList.teeth[toothIdx], patient->teethNotes[toothIdx]));

}

void ListPresenter::openDetails()
{
    if (m_selectedIndexes.size() == 1) openDetails(m_selectedIndexes[0]);
}

void ListPresenter::refreshProcedureView()
{
    if (view == nullptr) return;

    m_ambList.procedures.refreshTeethTemporary(m_ambList.teeth);

    view->setProcedures(m_ambList.procedures.list());
}

void ListPresenter::addProcedure()
{
    //making a copy to apply the procedures before adding a new one
    AmbList copy = m_ambList;
    for (auto& p : copy.procedures) {
        p.applyProcedure(copy.teeth);
    }

    ProcedureDialogPresenter p
    {
        copy,
        copy.teeth.getSelectedTeethPtr(m_selectedIndexes),
        patient->turns18At(),
        patient->canBePregnant(copy.getDate())
    };

    auto procedures = p.openDialog();

    if (procedures.empty()) return;

    m_ambList.procedures.addProcedures(procedures);

    if (view == nullptr) return;

    dynamicNhifConversion();

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

    dynamicNhifConversion();

    refreshProcedureView();

    makeEdited();

}

void ListPresenter::deleteProcedure(int index)
{
    m_ambList.procedures.removeProcedure(index);

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

void ListPresenter::showAppliedStatus()
{
    auto& procedures = m_ambList.procedures.list();

    if (procedures.empty()) {
        ModalDialogBuilder::showMessage(
            "Добавете поне една процедура"
        );
        return;
    }

    std::vector<HisSnapshot> result;

    result.reserve(procedures.size()+1);

    result.emplace_back();

    result.back().teeth = m_ambList.teeth;
    result.back().date = m_ambList.getDate();
    result.back().procedure_name = "Начален орален статус";

    for (auto& p : procedures)
    {
        result.emplace_back();

        auto& snapshot = result.back();

        snapshot.date = p.date;

        snapshot.affected_teeth = p.getArrayIndexes();
        snapshot.procedure_name = p.code.name();
        snapshot.procedure_diagnosis = p.diagnosis.icd.name();
        snapshot.procedure_note = p.notes;
        snapshot.financing = p.financingSource;
        snapshot.teeth = result[result.size()-2].teeth;

        p.applyProcedure(snapshot.teeth);
    }

    ModalDialogBuilder::showSnapshots(result);

}


void ListPresenter::addMedicalNotice()
{

    if (m_ambList.nrn.empty()) {
        ModalDialogBuilder::showMessage(
            "За да издадете медицинска бележка, "
            "първо изпратете амбулаторния лист към НЗИС"
        );
        return;
    }

    auto result = ModalDialogBuilder::openDialog(MedicalNotice());

    if (!result) return;

    m_ambList.medical_notices.push_back(*result);

    view->setAdditionalDocuments(m_ambList.referrals, m_ambList.medical_notices);

    if (!m_ambList.isNew()) {
        DbMedicalNotice::save(m_ambList.medical_notices, m_ambList.rowid);
    }

    sendMedicalNoticeToHis(m_ambList.medical_notices.size() - 1);

}

void ListPresenter::editMedicalNotice(int index)
{
    auto& nList = m_ambList.medical_notices;

    if (index < 0 || index >= nList.size()) return;

    auto result = ModalDialogBuilder::openDialog(nList[index]);

    if (!result) return;

    nList[index] = *result;

    if (isCurrent()) view->setAdditionalDocuments(m_ambList.referrals, m_ambList.medical_notices);


    if (!m_ambList.isNew()) {
        DbMedicalNotice::save(m_ambList.medical_notices, m_ambList.rowid);
    }
}


void ListPresenter::sendMedicalNoticeToHis(int index)
{
    if (m_ambList.nrn.empty()) 
    {
        ModalDialogBuilder::showMessage("За да издадете медицинска бележка, първо изпратете амбулаторния лист в НЗИС");
        return;
    }

    auto& notice = m_ambList.medical_notices[index];

    eMedicalNoticeIssue.sendRequest(notice, *patient, m_ambList.nrn,
        [=, this](const std::string& nrn) {

            if (nrn.empty()) return;

            m_ambList.medical_notices[index].nrn = nrn;
            
            DbMedicalNotice::save(m_ambList.medical_notices, m_ambList.rowid);

            ModalDialogBuilder::showMessage("Медицинската бележка е изпратено успешно");

            if (isCurrent()) view->setAdditionalDocuments(m_ambList.referrals, m_ambList.medical_notices);

        }
    );
}

void ListPresenter::removeMedicalNotice(int index)
{

    auto& nList = m_ambList.medical_notices;

    if (nList[index].nrn.empty()) {

        nList.erase(nList.begin() + index);

        if (!m_ambList.isNew()) {
            DbMedicalNotice::save(m_ambList.medical_notices, m_ambList.rowid);
        }

        if (isCurrent()) view->setAdditionalDocuments(m_ambList.referrals, m_ambList.medical_notices);

        return;
    }

    eMedicalNoticeCancel.sendRequest(nList[index].nrn, [=, this](bool success)
        {
            if (!success) return;

            m_ambList.medical_notices.erase(m_ambList.medical_notices.begin() + index);

            DbMedicalNotice::save(m_ambList.medical_notices, m_ambList.rowid);

            ModalDialogBuilder::showMessage("Медицинската бележка е анулирана успешно!");

            if (isCurrent()) view->setAdditionalDocuments(m_ambList.referrals, m_ambList.medical_notices);
        }
    );
}
void ListPresenter::addReferral(ReferralType type)
{

    if (!User::hasNhifContract()) {
        ModalDialogBuilder::showMessage(
            "За да създадете направление, трябва да попълните данните" 
            "\nна договора с НЗОК и кодът на специалността от насторйки"
        );

        return;
    }

    if (m_ambList.nrn.empty() && Referral::isNrnType(type)) {
        ModalDialogBuilder::showMessage(
            "За да издадете това направление, "
            "първо изпратете амбулаторния лист към НЗИС"
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

    view->setAdditionalDocuments(m_ambList.referrals, m_ambList.medical_notices);

    dynamicNhifConversion();
   
    refreshTabName();

    if (m_ambList.isNew()) return;
        
    DbReferral::saveReferrals(m_ambList.referrals, m_ambList.rowid);
    
    if (!m_ambList.referrals.back().isNrnType()) return;

    sendReferralToHis(m_ambList.referrals.size() - 1);
}

void ListPresenter::editReferral(int index)
{

    ReferralPresenter p(m_ambList, m_ambList.referrals[index]);

    auto result = p.openDialog();

    if (!result) return;

    m_ambList.referrals[index] = *result;

    view->setAdditionalDocuments(m_ambList.referrals, m_ambList.medical_notices);

    if (!m_ambList.isNew()) {
        DbReferral::saveReferrals(m_ambList.referrals, m_ambList.rowid);
    }
}


void ListPresenter::printReferral(int index)
{
    auto& ref = m_ambList.referrals[index];

    if (ref.isNrnType()) {
        
        if (ref.type != ReferralType::MDD4) return;

        if (!ref.isSentToHIS() &&
            !ModalDialogBuilder::askDialog(
                "Направлението не е изпратено към НЗИС. "
                "Сигурни ли сте, че искате да го принтирате?"
            )
        )
        {
            return;
        }
    }


    Print::referral(m_ambList.referrals[index], *patient.get(), m_ambList.getNumber());
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
            [=, this](const std::string& nrn) {

                if (nrn.empty()) return;

                m_ambList.referrals[index].nrn = nrn;

                refreshTabName();

                ModalDialogBuilder::showMessage("Направлението е изпратено успешно");

                DbReferral::saveReferrals(m_ambList.referrals, m_ambList.rowid);

                if (isCurrent()) {
                    view->setAdditionalDocuments(m_ambList.referrals, m_ambList.medical_notices);
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

        if(isCurrent()) view->setAdditionalDocuments(m_ambList.referrals, m_ambList.medical_notices);

        dynamicNhifConversion();

        refreshTabName();

        if (!m_ambList.isNew()) {
            DbReferral::saveReferrals(m_ambList.referrals, m_ambList.rowid);
        }

        return;
    }

    if (r.isSentToHIS()) {

        eReferralCancelService.sendRequest(r.nrn,
            [=, this](bool success) {

                if (!success) return;

                m_ambList.referrals.erase(m_ambList.referrals.begin() + index);

                ModalDialogBuilder::showMessage("Направлението е анулирано успешно");
                
                DbReferral::saveReferrals(m_ambList.referrals, m_ambList.rowid);

                dynamicNhifConversion();

                if(isCurrent()) view->setAdditionalDocuments(m_ambList.referrals, m_ambList.medical_notices);

                refreshTabName();

               
            }
        );
    }
}

void ListPresenter::setNhifData(const NhifSheetData& data)
{
    m_ambList.nhifData = data;
    makeEdited();
}

void ListPresenter::createInvoice()
{
    if (m_ambList.procedures.empty()) {
        TabPresenter::get().openInvoice(patient->rowid, {});
        return;
    }

    auto selectedProcedures = ModalDialogBuilder::selectProcedures(m_ambList.procedures.list(), FinancingSource::Patient);

    if (!selectedProcedures.has_value()) {
        return;
    }

    TabPresenter::get().openInvoice(patient->rowid, selectedProcedures.value());
}

void ListPresenter::createPerioMeasurment()
{
    TabPresenter::get().openPerio(*this->patient.get());
}

void ListPresenter::createPrescription()
{
    TabPresenter::get().openPerscription(*this->patient.get());
}

void ListPresenter::hisButtonPressed()
{
    //OPTIONAL INITIAL STATAUS PROCEDURE
    if (
       User::settings().autoStatus &&
       m_ambList.nrn.empty() &&
       m_ambList.procedures.size() &&
      !m_ambList.procedures.hasDentalExam()
    )
    {
        Procedure p;
        p.code = ProcedureCode("97011-00");
        p.diagnosis.icd = ICD10("Z01.2");
        p.date = m_ambList.getDate();
        p.notes = "ИЗХОДЕН ОРАЛЕН СТАТУС (автоматично генерирана дейност)";
        m_ambList.procedures.addProcedure(p);
        m_ambList.procedures.moveProcedure(
            m_ambList.procedures.list().size()-1,
            0
        );

        makeEdited();
        view->setProcedures(m_ambList.procedures.list());
        
    }

    //HIS Open
    if (m_ambList.nrn.empty()) {

        if (!save()) return;

        eDentalOpenService.sendRequest(
            m_ambList,
            *patient,
            [&](auto& nrn, auto& seqIdxPair, bool error) {

                if (nrn.empty()) {
                    return;
                }
                
                m_ambList.nrn = nrn;

                for (auto& [sequence, hisIdx] : seqIdxPair) {
                    
                    m_ambList.procedures[sequence].his_index = hisIdx;
                }

                m_ambList.his_updated = true;

                DbAmbList::update(m_ambList);

                refreshTabName();

                if (isCurrent())
                {
                    setHisButtonToView();
                    view->setProcedures(m_ambList.procedures.list());
                }

                if (error) {
                    //replace with auto-fetch when implemented
                    ModalDialogBuilder::showError("Амбулаторният лист не е синхронизиран с НЗИС! Моля анулирайте и го изпратете отново.");
                }
                else {
                    ModalDialogBuilder::showMessage("Денталният преглед е изпратен към НЗИС успешно");
                }
            }
        );

        return;
    }

    //HIS Update
    if (!m_ambList.his_updated)
    {
        if (!isValid()) return;

        eDentalAugmentService.sendRequest(
            m_ambList, 
            *patient, 
            DbAmbList::hasAutoStatus(m_ambList.nrn), 
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

                DbAmbList::setAutoStatus(m_ambList.nrn, false);
                ModalDialogBuilder::showMessage("Денталният преглед е коригиран успешно");
            }
        );

        return;
    }

    //HIS Cancel
    if (m_ambList.nrn.size()) {
        eDentalCancelService.sendRequest(m_ambList.nrn,
            [&](bool success) {

                if (!success) return;

                m_ambList.nrn.clear();

                m_ambList.procedures.clearRemovedProcedures();

                for (auto& p : m_ambList.procedures) p.his_index = 0;

                m_ambList.referrals.clear();
                m_ambList.medical_notices.clear();

                DbAmbList::update(m_ambList);

                refreshTabName();

                ModalDialogBuilder::showMessage("Денталният преглед е анулиран успешно");

                if (isCurrent())
                {
                    setDataToView();
                }
        });

        return;
    }


}


ListPresenter::~ListPresenter()
{
    if (isCurrent()){
        view->setPresenter(nullptr);
    }
}
