#include "ListPresenter.h"

#include "GlobalSettings.h"
#include "Database/DbReferral.h"
#include "Database/DbAmbList.h"
#include "Database/DbProcedure.h"
#include "Database/DbMedicalNotice.h"

#include "Model/User.h"
#include "Model/Validators/AmbListValidator.h"
#include "Model/FreeFunctions.h"

#include "Presenter/ProcedureDialogPresenter.h"
#include "Presenter/ProcedureEditorPresenter.h"
#include "Presenter/ReferralPresenter.h"
#include "Presenter/TabPresenter.h"
#include "Presenter/PatientHistoryPresenter.h"
#include "Presenter/DetailedStatusPresenter.h"
#include "Presenter/FiscalReceiptPresenter.h"

#include "View/Graphics/PaintHint.h"
#include "View/ModalDialogBuilder.h"
#include "View/Widgets/ProcedurePrintSelectDialog.h"
#include "View/Widgets/SignatureViewDialog.h"
#include "View/Widgets/ListView.h"
#include "View/Widgets/TabView.h"

#include "Printer/Print.h"
#include "Printer/FilePaths.h"

ListPresenter::ListPresenter(TabView* tabView, std::shared_ptr<Patient> patient, long long rowId)
    :
    TabInstance(tabView, TabType::AmbList, patient),
    patient_info(tabView->listView()->tileInfo(), patient),
    view(tabView->listView()),
    m_amblist(rowId ? DbAmbList::getListData(rowId) : DbAmbList::getNewAmbSheet(patient->rowid))
{
    patient_info.setParent(this);

    surf_presenter.setStatusControl(this);

    if (m_amblist.rowid) return;

    //the list is NEW:

    m_amblist.treatment_end = FreeFn::getTimeStampUTC();

    m_amblist.nhifData.isUnfavourable =
        User::practice().isUnfavourable() &&
        patient->city.isUnfav();

    m_amblist.lrn = FreeFn::getUuid();
    
}

void ListPresenter::statusChanged()
{
    m_checkModel = CheckModel(m_amblist.teeth.getSelectedTeethPtr(m_selectedIndexes));
    m_dsnCheckModel = CheckModel(m_amblist.teeth.getSelectedDsnPtr(m_selectedIndexes));

    view->setCheckModel(m_checkModel, m_dsnCheckModel);

    for (auto& idx : m_selectedIndexes)
    {
        view->repaintTooth(
            ToothPaintHint(m_amblist.teeth[idx], patient->teethNotes[idx])
        );
    }

    if (m_selectedIndexes.size() == 1)
        surf_presenter.setTooth(m_amblist.teeth[m_selectedIndexes[0]], patient->teethNotes[m_selectedIndexes[0]].size());

    makeEdited();
}

void ListPresenter::setHisButtonToView()
{
    if (m_amblist.nrn.empty()) {
        
        view->setHisButtonText(
            ListView::HisButtonProperties
            {
                .hideSpinBox = false,
                .buttonText = "Изпрати към НЗИС",
                .hoverText = "Изпрати към НЗИС"
            }
        );

        return;
    }

    if (m_amblist.nrn.size())
    {
        ListView::HisButtonProperties prop
        {
            .hideSpinBox = true,
            .buttonText = m_amblist.his_updated ? m_amblist.nrn : "Изпрати за корекция",
            .hoverText = m_amblist.his_updated ? "Анулирай" : "Изпрати за корекция"
        };

        view->setHisButtonText(prop);

        return;
    }
}

void ListPresenter::makeEdited()
{
    m_amblist.signature_bitmap = {};
    m_amblist.signature_data.clear();
    view->setSignature({});

    m_amblist.treatment_end = FreeFn::getTimeStampUTC();

    
    if (m_amblist.procedures.size()) {

        if (Date(m_amblist.treatment_end) < (m_amblist.procedures.end() - 1)->date)
        {
            m_amblist.treatment_end = (m_amblist.procedures.end() - 1)->date.to8601() + m_amblist.treatment_end.substr(10);
        }
    }
    else {

        if (Date(m_amblist.treatment_end) < m_amblist.getDate())
        {
            m_amblist.treatment_end = m_amblist.date;
		}
    }


    if (m_amblist.nrn.size()) {
        m_amblist.his_updated = false;

        if (isCurrent()) {
            setHisButtonToView();
        }

    }

    TabInstance::makeEdited();
}

void ListPresenter::printPrv(bool toPdf)
{

    if (toPdf && m_amblist.nrn.empty()) {
        ModalDialogBuilder::showMessage("Първо изпратете амбулаторния лист към НЗИС");
        return;
    }

    std::vector<Procedure> selectedProcedures;
    bool printReferrals = false;

    if (m_amblist.procedures.size() || m_amblist.referrals.size())
    {
        ProcedurePrintSelectDialog dialog(m_amblist.procedures.list(), m_amblist.referrals);

        for (auto& p : m_amblist.procedures) {

            if (p.isNhif()) {
                dialog.selectFinancingSource(FinancingSource::NHIF);
                break;
            }
        }

        if (dialog.exec() == QDialog::Rejected) {
            return;
        }

        auto selectedIndexes = dialog.selectedProcedures();

        for (auto idx : selectedIndexes) {
            selectedProcedures.push_back(m_amblist.procedures.at(idx));
        }

        printReferrals = dialog.printReferrals();

    }

    bool nhifProceduresOnly =
        std::find_if(selectedProcedures.begin(), selectedProcedures.end(),
            [&](const Procedure& p) { return p.financingSource != FinancingSource::NHIF; }
        ) == selectedProcedures.end();

    bool printNhif = printReferrals || selectedProcedures.empty() || nhifProceduresOnly;

    if (printNhif && selectedProcedures.size() > 6) {
        printNhif = !ModalDialogBuilder::askDialog(
            "Избрали сте повече от 6 процедури. "
            "Желаете ли да бъде принтиран амбулаторен лист с повече позиции за процедурите? "
        );
    }

    std::string filepath;

    //get dir
    if (toPdf) {
        filepath = FilePaths::get(m_amblist, *patient, printNhif);
        if (filepath.empty()) return;
    }


    bool success = Print::ambList(m_amblist, *patient, printNhif, selectedProcedures, printReferrals, filepath);

    if (!toPdf) return;

    if (!User::signatureTablet().isPDFconfigured()) {
        
        if (ModalDialogBuilder::askDialog(
            "Файлът е запазен успешно. Желаете ли да отворите директорията?"
        ))
        {
            ModalDialogBuilder::openExplorer(filepath);   
        }

        return;
    }

    if (ModalDialogBuilder::askDialog("Файлът е запазен успешно. Желаете ли да го подпишете с графологичен таблет?")){
        
        User::signatureTablet().signPdf(filepath);
        
        return;
	}

}

void ListPresenter::fetchListProcedures(const std::string& nrn)
{
    eDentalFetchService.sendRequest(nrn,
        [&](const AmbList& result, const Patient& patient) {
                
		    	m_amblist.nrn = result.nrn;
                m_amblist.procedures = result.procedures;
				m_amblist.his_updated = true;
                
				DbAmbList::update(m_amblist);

                refreshTabName();

                if (isCurrent())
                {
                    setHisButtonToView();
                    view->setProcedures(m_amblist.procedures.list());
                }

                ModalDialogBuilder::showMessage("Денталният преглед е изпратен към НЗИС успешно");
        }
	);
}

void ListPresenter::dynamicNhifConversion()
{
    if (User::hasNhifContract()) {

        view->setNhifData(m_amblist.nhifData, User::practice().isUnfavourable());
    }
    else
    {
        view->hideNhifSheetData();
    }

}

void ListPresenter::patientDataChanged()
{
    if (!User::practice().isUnfavourable()) { return; }
        
    if (patient->city.isUnfav() == m_amblist.nhifData.isUnfavourable) { return; }
    
    m_amblist.nhifData.isUnfavourable = !m_amblist.nhifData.isUnfavourable;

    //marking the list as edited only if the unfavourable condition has been changed
    view->setNhifData(m_amblist.nhifData, true);
    makeEdited();
}

void ListPresenter::setSignature(const std::vector<unsigned char> sig_bitmap, const std::string sig_data)
{
    m_amblist.signature_bitmap = sig_bitmap;
    m_amblist.signature_data = sig_data;

    DbAmbList::update(m_amblist);

    refreshTabName();

    if(!isCurrent()){ return; }

    view->setSignature(sig_bitmap);

}

bool ListPresenter::isValid()
{
    bool nonExamProcedureFound = false;

    for (auto& p : m_amblist.procedures)
    {

        switch (p.code.type()) {

            case ProcedureType::FullExam:

                if (nonExamProcedureFound) {

                    ModalDialogBuilder::showError(
                        "С оглед на правилната хронология на пациентското досие в НЗИС, "
                        "процедурата отразяваща зъбния статус трябва да бъде винаги първа по ред. "
                        "Преместете я (при необходимост - редактирайте датата ѝ)."
                    );

                    return false;
                }

                break;

            case ProcedureType::General:
            case ProcedureType::ToothNonSpecific:
                continue;

            default: nonExamProcedureFound = true;
        }

    }


    for (auto& p : m_amblist.procedures)
    {
        auto& idx = p.getToothIndex();

        if (idx.supernumeral && !m_amblist.teeth[idx.index][Dental::HasSupernumeral])
        {
            ModalDialogBuilder::showError(
            "За да запишете процерура на свръхброен зъб, отбележете го като такъв в статуса!"
            );
            return false;
        }

        if (p.code.getScope() == ProcedureScope::SingleTooth
            && m_amblist.teeth.at(p.getToothIndex()).noData())
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

    auto date = m_amblist.getDate();

    for (auto& p : m_amblist.procedures)
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

    if (m_amblist.isNhifSheet() && !patient->PISHistory.has_value()) {
        ModalDialogBuilder::showMessage(
            "Не са заредени данни от ПИС. "
            "Листът ще бъде валидиран само "
            "спрямо съществуващите записи в локалната база данни");
    }


    AmbListValidator checker(m_amblist, *patient);

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

    if (m_amblist.isNew()) {
        n.header += "Нов амб. лист ";
    }
    else if (m_amblist.nrn.size()) {
        n.header += "Амб. лист ";
        n.header += m_amblist.nrn;
    }
    else if (m_amblist.number){
        n.header += "Амб. лист №";
        n.header += std::to_string(m_amblist.number);
    }
    else {
        n.header += "Амбулаторен лист ";
    }

    n.footer = patient->FirstName;
    n.footer += " ";
    n.footer += patient->LastName;

    n.indicatorColor = patient->colorNameRgb;

    if (m_amblist.isNhifSheet()) {
        n.footer_icon = CommonIcon::NHIF;
    }

    if (m_amblist.nrn.empty() && m_amblist.signature_bitmap.size()) {
        n.header_icon = CommonIcon::SIGNATURE;
    }

    if (m_amblist.nrn.size()) {
        n.header_icon = m_amblist.signature_bitmap.size() ? CommonIcon::SIGNATURE : CommonIcon::HIS;

        if (!m_amblist.his_updated) {
            n.header_icon = CommonIcon::HISGRAY;
        }
    }

    return n;
}

long long ListPresenter::rowID() const
{
    return m_amblist.rowid;
}

bool ListPresenter::save()
{
    if (!requiresSaving()) return true;

    if (!isValid()) return false;

    auto d = m_amblist.getDate();

    if (m_amblist.isNew())
    {
        m_amblist.rowid = DbAmbList::insert(m_amblist, patient->rowid);
    }
    else
    {
		if (m_amblist.signature_bitmap.size()
            && !ModalDialogBuilder::askDialog("Промяната на амбулаторния лист ще направи подписа невалиден. Сигурни ли сте, че искате да продължите?")
        ){
            return false;
		}

        m_amblist.signature_bitmap.clear();

        view->setSignature({});

        DbAmbList::update(m_amblist);
    }

    edited = false;

    refreshTabName();

    return true;

}

bool ListPresenter::isNew()
{
    return m_amblist.isNew();
}

void ListPresenter::print()
{
    printPrv(false);
}

void ListPresenter::pdfPrint()
{
    printPrv(true);
}

void ListPresenter::setDataToView()
{
    view->setPresenter(this);
    
    patient_info.setDate(m_amblist.getDate());

    patient_info.setCurrent(true);

    setHisButtonToView();

    view->setDateTime(m_amblist.date);

	view->setSignature(m_amblist.signature_bitmap);

    surf_presenter.setStatusControl(this);
    surf_presenter.setView(view->surfacePanel());
    view->surfacePanel()->setPresenter(&surf_presenter);

    for (int i = 0; i < 32; i++)
    {
        view->repaintTooth(ToothPaintHint(m_amblist.teeth[i], patient->teethNotes[i]));
    }

    view->setNotes(patient->teethNotes);
    
    view->setAdditionalDocuments(m_amblist.referrals, m_amblist.medical_notices);

    refreshProcedureView();
    dynamicNhifConversion();

    view->setSelectedTeeth(m_selectedIndexes);

    view->focusTeethView();

	if (!firstFocus){ return; }

    bool querySent = true; //prevents multiple PKCS11 prompts

    if (User::settings().getPisHistoryAuto && User::hasNhifContract() && !patient->PISHistory) {

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

    if (User::settings().getHisHistoryAuto && !patient->HISHistory) {
            
        auto callback = [&](const std::optional<std::vector<Procedure>>& result, const std::vector<HisSnapshot>& snapshots) {

            if (!result) return;
 
            auto& procedures = result.value();

            patient->HISHistory = procedures;
  
            if (!m_amblist.isNew()) return;

            if(snapshots.empty()) return;

//              m_amblist.teeth.copyOnlyOnUnknown(snapshots.back().teeth);

            auto& lastHisSnapshotDate = snapshots.back().date;

            auto lastDbProcedureDate = DbProcedure::getLastProcedureDate(patient->rowid);

            if(m_amblist.teeth.noData() || (
                lastHisSnapshotDate > lastDbProcedureDate
                && ModalDialogBuilder::askDialog(
                    "В НЗИС е открит по-актуален орален статус. Желаете ли да го заредите?"
                )
                )
            ) {
                m_amblist.teeth.copyFromHis(snapshots.back().teeth);
            }

            for (int i = 0; i < 32; i++)
            {
                view->repaintTooth(ToothPaintHint{ m_amblist.teeth[i], patient->teethNotes[i] });
            }

                
        };

        eDentalGetStatusAndProceduresService.sendRequest(*patient, false, callback);

    }

    firstFocus = false;
}


void ListPresenter::setAmbDateTime(const std::string& datetime)
{
    m_amblist.date = datetime;

    if (!User::practice().generateMonthlySheets()) {

        auto pDate = Date(datetime);

        for (auto& p : m_amblist.procedures) {
            p.date = pDate;
        }

        view->setProcedures(m_amblist.procedures.list());
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

    auto& teeth = m_amblist.teeth;

    for (auto idx : m_selectedIndexes)
    {

        auto& tooth = m_amblist.teeth[idx];

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
        view->repaintTooth(ToothPaintHint{ m_amblist.teeth[i], patient->teethNotes[i] });
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
        case Dental::StatusType::DefectiveRestoration: state = checkModel.defRestoStatus[code] != CheckState::checked; break;
        case Dental::StatusType::NonCariesLesion: state = checkModel.nonCariesStatus[code] != CheckState::checked; break;
        case Dental::StatusType::Mobility: state = checkModel.mobilityStatus[code] != CheckState::checked; break;
    }

    m_amblist.teeth.setStatus(m_selectedIndexes, t, code, state, supernumeral);

    if (t == Dental::StatusType::General)
    {
        if (code == Dental::Temporary) {
            refreshProcedureView(); //updates the teeth num
        }

        for (int i = 0; i < 32; i++) {
            view->repaintTooth(ToothPaintHint(m_amblist.teeth[i], patient->teethNotes[i]));
        }
    }

    statusChanged();
}

void ListPresenter::setSelectedTeeth(const std::vector<int>& SelectedIndexes)
{
    m_selectedIndexes = SelectedIndexes;

    m_checkModel = CheckModel(m_amblist.teeth.getSelectedTeethPtr(SelectedIndexes));
    m_dsnCheckModel = CheckModel(m_amblist.teeth.getSelectedDsnPtr(SelectedIndexes));

    view->setCheckModel(m_checkModel, m_dsnCheckModel);

    if(m_selectedIndexes.size() == 1){
        surf_presenter.setTooth(m_amblist.teeth[m_selectedIndexes[0]], patient->teethNotes[m_selectedIndexes[0]].size());
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

    PatientHistoryPresenter pr(*patient);

    auto result = pr.openDialog();

	view->setNotes(patient->teethNotes);

    if (std::holds_alternative<std::monostate>(result)) {
        return;
    }

    if (std::holds_alternative<std::vector<Procedure>*>(result)) {
        for (auto& nhifProcedure : patient->PISHistory.value()) {
            nhifProcedure.applyPISProcedure(m_amblist.teeth);
        }
    }

    if (std::holds_alternative<ToothContainer>(result)) {
        m_amblist.teeth.copyFromHis(std::get<ToothContainer>(result));
    }


    for (int i = 0; i < 32; i++) {
        view->repaintTooth(ToothPaintHint(m_amblist.teeth[i], patient->teethNotes[i]));
    }

    statusChanged();
}

void ListPresenter::openDetails(int toothIdx)
{
    if (toothIdx < 0 || toothIdx > 31) return;

    DetailedStatusPresenter d(toothIdx, *patient);

    d.open();

    patient->teethNotes[toothIdx] = d.getNote();
    view->setNotes(patient->teethNotes);
}

void ListPresenter::openDetails()
{
    if (m_selectedIndexes.size() != 1) {
        return;
    }
    
    openDetails(m_selectedIndexes[0]);
}

void ListPresenter::refreshProcedureView()
{
    if (view == nullptr) return;

    m_amblist.procedures.refreshTeethTemporary(m_amblist.teeth);

    view->setProcedures(m_amblist.procedures.list());
}

void ListPresenter::addProcedure()
{
    //making a copy to apply the procedures before adding a new one
    AmbList copy = m_amblist;
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

    m_amblist.procedures.addProcedures(procedures);

    if (view == nullptr) return;

    dynamicNhifConversion();

    refreshProcedureView();

    makeEdited();

}

void ListPresenter::editProcedure(int index)
{
    if (index < 0 || index >= m_amblist.procedures.size()) return;

    auto& m_for_edit = m_amblist.procedures.at(index);

    ProcedureEditorPresenter p(m_for_edit, patient->turns18At());

    auto result = p.openDialog();

    if (!result) return;

    auto& m = result.value();

    m_amblist.procedures.replaceProcedure(m, index);

    dynamicNhifConversion();

    refreshProcedureView();

    makeEdited();

}

void ListPresenter::deleteProcedure(int index)
{
    m_amblist.procedures.removeProcedure(index);

    refreshProcedureView();

    dynamicNhifConversion();


    makeEdited();
}

void ListPresenter::moveProcedure(int from, int to)
{
    if(!m_amblist.procedures.moveProcedure(from, to)) return;

    makeEdited();

    view->setProcedures(m_amblist.procedures.list());
}

void ListPresenter::showAppliedStatus()
{
    auto& procedures = m_amblist.procedures.list();

    if (procedures.empty()) {
        ModalDialogBuilder::showMessage(
            "Добавете поне една процедура"
        );
        return;
    }

    std::vector<HisSnapshot> result;

    result.reserve(procedures.size()+1);

    result.emplace_back();

    result.back().teeth = m_amblist.teeth;
    result.back().date = m_amblist.getDate();
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

void ListPresenter::showSignature()
{
    SignatureViewDialog d(m_amblist.signature_bitmap, m_amblist.signature_data);
    d.exec();

    if(d.removeSignature()){
        setSignature({}, "");
	}
}


void ListPresenter::addMedicalNotice()
{

    if (m_amblist.nrn.empty()) {
        ModalDialogBuilder::showMessage(
            "За да издадете медицинска бележка, "
            "първо изпратете амбулаторния лист към НЗИС"
        );
        return;
    }

    auto result = ModalDialogBuilder::openDialog(MedicalNotice());

    if (!result) return;

    m_amblist.medical_notices.push_back(*result);

    view->setAdditionalDocuments(m_amblist.referrals, m_amblist.medical_notices);

    if (!m_amblist.isNew()) {
        DbMedicalNotice::save(m_amblist.medical_notices, m_amblist.rowid);
    }

    sendMedicalNoticeToHis(m_amblist.medical_notices.size() - 1);

}

void ListPresenter::editMedicalNotice(int index)
{
    auto& nList = m_amblist.medical_notices;

    if (index < 0 || index >= nList.size()) return;

    auto result = ModalDialogBuilder::openDialog(nList[index]);

    if (!result) return;

    nList[index] = *result;

    if (isCurrent()) view->setAdditionalDocuments(m_amblist.referrals, m_amblist.medical_notices);


    if (!m_amblist.isNew()) {
        DbMedicalNotice::save(m_amblist.medical_notices, m_amblist.rowid);
    }
}


void ListPresenter::sendMedicalNoticeToHis(int index)
{
    if (m_amblist.nrn.empty()) 
    {
        ModalDialogBuilder::showMessage("За да издадете медицинска бележка, първо изпратете амбулаторния лист в НЗИС");
        return;
    }

    auto& notice = m_amblist.medical_notices[index];

    eMedicalNoticeIssue.sendRequest(notice, *patient, m_amblist.nrn,
        [=, this](const std::string& nrn) {

            if (nrn.empty()) return;

            m_amblist.medical_notices[index].nrn = nrn;
            
            DbMedicalNotice::save(m_amblist.medical_notices, m_amblist.rowid);

            ModalDialogBuilder::showMessage("Медицинската бележка е изпратено успешно");

            if (isCurrent()) view->setAdditionalDocuments(m_amblist.referrals, m_amblist.medical_notices);

        }
    );
}

void ListPresenter::removeMedicalNotice(int index)
{

    auto& nList = m_amblist.medical_notices;

    if (nList[index].nrn.empty()) {

        nList.erase(nList.begin() + index);

        if (!m_amblist.isNew()) {
            DbMedicalNotice::save(m_amblist.medical_notices, m_amblist.rowid);
        }

        if (isCurrent()) view->setAdditionalDocuments(m_amblist.referrals, m_amblist.medical_notices);

        return;
    }

    eMedicalNoticeCancel.sendRequest(nList[index].nrn, [=, this](bool success)
        {
            if (!success) return;

            m_amblist.medical_notices.erase(m_amblist.medical_notices.begin() + index);

            DbMedicalNotice::save(m_amblist.medical_notices, m_amblist.rowid);

            ModalDialogBuilder::showMessage("Медицинската бележка е анулирана успешно!");

            if (isCurrent()) view->setAdditionalDocuments(m_amblist.referrals, m_amblist.medical_notices);
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

    if (m_amblist.nrn.empty() && Referral::isNrnType(type)) {
        ModalDialogBuilder::showMessage(
            "За да издадете това направление, "
            "първо изпратете амбулаторния лист към НЗИС"
        );
        return;
    }

    if (type == ReferralType::Form3 || type == ReferralType::Form3A) {
        for (auto& r : m_amblist.referrals)
        {
            if (r.type == ReferralType::Form3 || type == ReferralType::Form3A) {
                ModalDialogBuilder::showMessage(
                    "Позволено е максимум едно направление от тип бл.3 или бл.3А"
                );
                return;
            }
        }
    }
    /*
    else if(type == ReferralType::MDD4)
    {
        int mddCounter = 0;

        for (auto& r : m_amblist.referrals)
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
    */
    else
    {
        for (auto& r : m_amblist.referrals)
        {
            if (r.type == ReferralType::MH119) {

                ModalDialogBuilder::showMessage(
                    "Позволено е максимум по едно направление от тип 119 МЗ"
                );
                return;
            }
        }

    }

    ReferralPresenter p(m_amblist, type);

    auto result = p.openDialog();

    if (!result) return;

    m_amblist.referrals.push_back(*result);

    view->setAdditionalDocuments(m_amblist.referrals, m_amblist.medical_notices);

    dynamicNhifConversion();
   
    refreshTabName();

    if (m_amblist.isNew()) return;
        
    DbReferral::saveReferrals(m_amblist.referrals, m_amblist.rowid);
    
    if (!m_amblist.referrals.back().isNrnType()) return;

    sendReferralToHis(m_amblist.referrals.size() - 1);
}

void ListPresenter::editReferral(int index)
{

    ReferralPresenter p(m_amblist, m_amblist.referrals[index]);

    auto result = p.openDialog();

    if (!result) return;

    m_amblist.referrals[index] = *result;

    view->setAdditionalDocuments(m_amblist.referrals, m_amblist.medical_notices);

    if (!m_amblist.isNew()) {
        DbReferral::saveReferrals(m_amblist.referrals, m_amblist.rowid);
    }
}


void ListPresenter::printReferral(int index)
{
    auto& ref = m_amblist.referrals[index];

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


    Print::referral(m_amblist.referrals[index], *patient.get(), m_amblist.getNumber());
}

void ListPresenter::sendReferralToHis(int index)
{
    auto& ref = m_amblist.referrals[index];

    if (!ref.isNrnType()) return;

    if (m_amblist.nrn.empty()) {
        ModalDialogBuilder::showMessage("За да издадете направление, първо изпратете амбулаторния лист в НЗИС");
        return;
    }

    if (ref.nrn.empty())
    {
        eReferralIssueService.sendRequest(
            m_amblist.nrn,
            *patient.get(),
            m_amblist.referrals[index],
            [=, this](const std::string& nrn) {

                if (nrn.empty()) return;

                m_amblist.referrals[index].nrn = nrn;

                refreshTabName();

                ModalDialogBuilder::showMessage("Направлението е изпратено успешно");

                DbReferral::saveReferrals(m_amblist.referrals, m_amblist.rowid);

                if (isCurrent()) {
                    view->setAdditionalDocuments(m_amblist.referrals, m_amblist.medical_notices);
                }
            }
        );

        return;
    }
}


void ListPresenter::removeReferral(int index)
{
    auto& r = m_amblist.referrals[index];

    if (!r.isNrnType() || !r.isSentToHIS())
    {
        auto& rList = m_amblist.referrals;

        rList.erase(rList.begin() + index);

        if(isCurrent()) view->setAdditionalDocuments(m_amblist.referrals, m_amblist.medical_notices);

        dynamicNhifConversion();

        refreshTabName();

        if (!m_amblist.isNew()) {
            DbReferral::saveReferrals(m_amblist.referrals, m_amblist.rowid);
        }

        return;
    }

    if (r.isSentToHIS()) {

        eReferralCancelService.sendRequest(r.nrn,
            [=, this](bool success) {

                if (!success) return;

                m_amblist.referrals.erase(m_amblist.referrals.begin() + index);

                ModalDialogBuilder::showMessage("Направлението е анулирано успешно");
                
                DbReferral::saveReferrals(m_amblist.referrals, m_amblist.rowid);

                dynamicNhifConversion();

                if(isCurrent()) view->setAdditionalDocuments(m_amblist.referrals, m_amblist.medical_notices);

                refreshTabName();

               
            }
        );
    }
}

void ListPresenter::setNhifData(const NhifSheetData& data)
{
    m_amblist.nhifData = data;
    makeEdited();
}

void ListPresenter::createInvoice()
{
    if (m_amblist.procedures.empty()) {
        TabPresenter::get().openInvoice(patient->rowid, {});
        return;
    }

    auto selectedProcedures = ModalDialogBuilder::selectProcedures(m_amblist.procedures.list(), FinancingSource::Patient);

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

void ListPresenter::printDeclarations()
{

    std::vector<std::string> optionName = {
        "Информирано съгласие",
        "Декларация за GDPR"
    };

    std::vector<FilePaths::DeclarationType> optionValue = {
        FilePaths::Consent,
        FilePaths::GDPR
    };

    if (User::hasNhifContract()) {

        optionName = {
            "Информирано съгласие",
            "Декларация за тотални протези",
            "Декларация за валидна здравна книжка",
            "Декларация за GDPR"
        };
    
        optionValue = {
            FilePaths::Consent,
            FilePaths::Denture,
            FilePaths::HIRBNo,
            FilePaths::GDPR
        };
    }

    //choose declaration
    int result = ModalDialogBuilder::openButtonDialog(optionName, "Изберете декларация");

    if (result == -1) return;

    auto declarationType = optionValue[result];

    //choose signing, if enabled
    bool pdfSign = false;
    std::string filepath;

    if(User::signatureTablet().isPDFconfigured())
    {

        int dResult = ModalDialogBuilder::openButtonDialog({
            "Подписване с пен таблет",
            "Принтиране"
            } , 
            "Изберете формат"
        );

        if (dResult == -1) {
            return;
        }
        else if(dResult == 0) {

            pdfSign = true;

            filepath = FilePaths::get(declarationType, *patient);

            if (filepath.empty()) return;
        }
    }

    //executing

    bool success = false;

    switch(declarationType)
    {
        case FilePaths::DeclarationType::Denture: 
            success = Print::printDentureDeclaration(patient.get(), &m_amblist, filepath); break;
        case FilePaths::DeclarationType::HIRBNo: 
            success = Print::printHirbNoDeclaration(patient.get(), filepath); break;
        case FilePaths::DeclarationType::Consent: 
            success = Print::consent(*patient, filepath); break;
        case FilePaths::DeclarationType::GDPR: 
            success = Print::gdpr(*patient, filepath); break;
        default: return;
    }

    if (success && pdfSign) {
        User::signatureTablet().signPdf(filepath);
    }

}

void ListPresenter::hisButtonPressed()
{
    
	if (m_amblist.nrn.empty() || !m_amblist.his_updated)
    {
        sendToHis();
        return;
	}

    //HIS Cancel
    if (m_amblist.nrn.size()) { cancelHisAmbList(); }
}

void ListPresenter::cancelHisAmbList()
{
    eDentalCancelService.sendRequest(m_amblist.nrn,
        [&](bool success) {

            if (!success) return;

            m_amblist.nrn.clear();
            m_amblist.signature_bitmap.clear();
            m_amblist.signature_data.clear();
            m_amblist.procedures.clearRemovedProcedures();

            for (auto& p : m_amblist.procedures) {
                p.his_index = 0;
            }

            m_amblist.referrals.clear();
            m_amblist.medical_notices.clear();

            DbAmbList::update(m_amblist);

            refreshTabName();

            ModalDialogBuilder::showMessage("Денталният преглед е анулиран успешно");

            if (isCurrent())
            {
                setDataToView();
            }
        });
}

void ListPresenter::sendToHis(bool patientIsSigner)
{
    //OPTIONAL INITIAL STATAUS PROCEDURE

    if (
        User::settings().autoStatus &&
        m_amblist.nrn.empty() &&
        m_amblist.procedures.size() &&
        !m_amblist.procedures.hasDentalExam()
        )
    {
        Procedure p;
        p.code = ProcedureCode("97011-00");
        p.diagnosis.icd = ICD10("Z01.2");
        p.date = m_amblist.getDate();
        p.notes = "ИЗХОДЕН ОРАЛЕН СТАТУС (автоматично генерирана дейност)";
        m_amblist.procedures.addProcedure(p);
        m_amblist.procedures.moveProcedure(
            m_amblist.procedures.list().size() - 1,
            0
        );

        makeEdited();
        view->setProcedures(m_amblist.procedures.list());
    }

    if (m_amblist.treatment_end.empty()) {
        m_amblist.treatment_end = FreeFn::getTimeStampUTC();
    }

    //HIS Open
    if (m_amblist.nrn.empty()) {

        if (!save()) return;

        eDentalOpenService.sendRequest(
            m_amblist,
            *patient,
            patientIsSigner,
            [&](auto& nrn, auto& seqIdxPair, bool outOfSync) {

                if (nrn.empty()) {
                    return;
                }

                if (outOfSync) {
                    fetchListProcedures(nrn);
                    return;
                }

                m_amblist.nrn = nrn;

                for (auto& [sequence, hisIdx] : seqIdxPair) {

                    m_amblist.procedures[sequence].his_index = hisIdx;
                }

                m_amblist.his_updated = !outOfSync;

                DbAmbList::update(m_amblist);

                refreshTabName();

                if (isCurrent())
                {
                    setHisButtonToView();
                    view->setProcedures(m_amblist.procedures.list());
                }

                ModalDialogBuilder::showMessage("Денталният преглед е изпратен към НЗИС успешно");

            },
            [&](const std::vector<unsigned char>& sig_bitmap, const std::string& sig_data) { setSignature(sig_bitmap, sig_data); }
        );

        return;
    }

    //HIS Update
    if (!m_amblist.his_updated || !patientIsSigner)
    {
        if (!isValid()) return;

        eDentalAugmentService.sendRequest(
            m_amblist,
            *patient,
            patientIsSigner,
            DbAmbList::hasAutoStatus(m_amblist.nrn),
            [&](auto& procedureIdx)
            {
                m_amblist.his_updated = true;
                m_amblist.procedures.clearRemovedProcedures();

                for (auto& [sequence, hisIdx] : procedureIdx)
                {
                    m_amblist.procedures[sequence].his_index = hisIdx;
                }

                DbAmbList::update(m_amblist);

                edited = false;

                refreshTabName();

                if (isCurrent())
                {
                    setHisButtonToView();
                    view->setProcedures(m_amblist.procedures.list());
                }

                DbAmbList::setAutoStatus(m_amblist.nrn, false);
                ModalDialogBuilder::showMessage("Денталният преглед е коригиран успешно");
            },
            [&](const std::vector<unsigned char>& sig_bitmap, const std::string& sig_data) {

                setSignature(sig_bitmap, sig_data);
                DbAmbList::update(m_amblist);
                edited = false;
            }
        );

        return;
    }
}


ListPresenter::~ListPresenter()
{
    if (isCurrent()){
        view->setPresenter(nullptr);
    }
}
