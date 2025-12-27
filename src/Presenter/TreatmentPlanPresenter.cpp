#include "TreatmentPlanPresenter.h"

#include "Database/DbTreatmentPlan.h"

#include "Model/User.h"

#include "ProcedureDialogPresenter.h"
#include "Presenter/PatientHistoryPresenter.h"

#include "View/Widgets/MultilineDialog.h"
#include "View/Widgets/TabView.h"
#include "View/Widgets/TreatmentPlanView.h"
#include "View/Widgets/PriceInputDialog.h"
#include "View/Widgets/PlannedProcedureDialog.h"
#include "Printer/Print.h"
#include "Printer/FilePaths.h"

TreatmentPlanPresenter::TreatmentPlanPresenter(TabView* tabView, std::shared_ptr<Patient> patient, long long rowid)
    :
    TabInstance(tabView, TabType::TreatmentPlan, patient),
    patient_info(tabView->treatmentPlanView()->tileInfo(), patient),
    view(tabView->treatmentPlanView())
{
    m_treatmentPlan = DbTreatmentPlan::get(rowid);

    if(!rowid){
        rowid = DbTreatmentPlan::getActivePlan(patient->rowid);

        if(rowid){
            ModalDialogBuilder::showMessage(
                "За този пациент е открит активен план за лечение. "
                "Ако искате да създадете нов, маркирайте текущия като изпълнен."
            );

            m_treatmentPlan = DbTreatmentPlan::get(rowid);
        }
    }

    if(m_treatmentPlan.rowid){

        m_selection.first = m_treatmentPlan.stages.size()-1;

        if(m_selection.first > -1){
            m_selection.second = m_treatmentPlan.stages[m_selection.first].plannedProcedures.size()-1;
        }

        return;
    }

    m_treatmentPlan.teeth = DbAmbList::getStatus(patient->rowid, m_treatmentPlan.date);
}


void TreatmentPlanPresenter::setDataToView()
{
    view->setPresenter(this);

    patient_info.setCurrent(true);

    setCompletedProcedures();

    view->setTreatmentPlan(m_treatmentPlan);

    view->setSelection(m_selection);

    setTeethToView();
}

void TreatmentPlanPresenter::setTeethToView()
{
    auto copy = m_treatmentPlan.teeth;

    std::vector<int> affectedTeeth;
    affectedTeeth.reserve(32);

    for(int i = 0; i <= m_selection.first; i++){

        affectedTeeth.clear();

        int procedureCount = i == m_selection.first ? m_selection.second
        : m_treatmentPlan.stages[i].plannedProcedures.size()-1;

        for(int j = 0; j <= procedureCount; j++){
           auto procedure = m_treatmentPlan
                .stages[i]
                .plannedProcedures[j]
                .getProcedure();

            procedure.applyProcedure(copy);
            affectedTeeth = procedure.getArrayIndexes();
        }
    }

    for(auto& t : copy){
        view->repaintTooth(ToothPaintHint(t));
        view->setAffectedTeeth(affectedTeeth);
    }
}

void TreatmentPlanPresenter::setCompletedProcedures()
{
    auto completed = DbTreatmentPlan::getCompletedProcedures(patient->rowid);

    for(auto& s : m_treatmentPlan.stages)
        for (auto& p : s.plannedProcedures)
            p.isCompleted = completed.count(p.rowid);
}

TreatmentPlan::Stage *TreatmentPlanPresenter::getCurrentStage()
{
    if(m_selection.first < 0) return nullptr;

    if(m_selection.first >= m_treatmentPlan.stages.size()) return nullptr;

    return &m_treatmentPlan.stages[m_selection.first];

}

TreatmentPlan::PlannedProcedure *TreatmentPlanPresenter::getCurrentProcedure()
{
    if(m_selection.second < 0) return nullptr;

    auto stage = getCurrentStage();

    if(!stage) return nullptr;

    if(m_selection.second >= stage->plannedProcedures.size()) return nullptr;

    return &stage->plannedProcedures[m_selection.second];
}

TreatmentPlan::Stage *TreatmentPlanPresenter::getConclusion()
{
    if(!m_treatmentPlan.lastStageIsConclusion) return nullptr;

    if(m_treatmentPlan.stages.empty()) return nullptr;

    return &m_treatmentPlan.stages.back();
}

void TreatmentPlanPresenter::prepareDerivedForSwitch()
{
    view->setPresenter(nullptr);
    patient_info.setCurrent(false);
}

void TreatmentPlanPresenter::patientDataChanged()
{
    patient_info.refreshPatientData();
}

bool TreatmentPlanPresenter::isNew()
{
    return !m_treatmentPlan.rowid;
}

TabName TreatmentPlanPresenter::getTabName()
{
    TabName n;

    if (isNew()) {
        n.header += "Нов лечебен план";
    }
    else {
        n.header += "Лечебен план ";
        n.header += m_treatmentPlan.date.toBgStandard();
    }

    n.footer = patient->FirstName;
    n.footer += " ";
    n.footer += patient->LastName;

    n.indicatorColor = patient->colorNameRgb;

    n.header_icon = CommonIcon::TREATMENTPLAN;

    return n;
}

bool TreatmentPlanPresenter::save()
{
    if(!requiresSaving()) return true;

    bool success =  isNew() ?
                       DbTreatmentPlan::insert(m_treatmentPlan, patient->rowid)
                           :
                       DbTreatmentPlan::update(m_treatmentPlan, m_deleted_procedures);

    if(!success) return false;

    m_deleted_procedures.clear();

    edited = false;

    refreshTabName();

    return true;

}

void TreatmentPlanPresenter::print()
{
    save();

    Print::treatmentPlan(m_treatmentPlan, *patient);
}

void TreatmentPlanPresenter::pdfPrint()
{
    save();

    auto filepath = FilePaths::get(m_treatmentPlan, *patient);
    if (filepath.empty()) return;

    bool success = Print::treatmentPlan(m_treatmentPlan, *patient, filepath);

    if(!success) return;

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

long long TreatmentPlanPresenter::rowID() const
{
    return m_treatmentPlan.rowid;
}

void TreatmentPlanPresenter::loadStatus()
{
    PatientHistoryPresenter dialog(*patient);
    auto result = dialog.openDialog(true, true);

    if(std::holds_alternative<ToothContainer>(result)){
        m_treatmentPlan.teeth = std::get<ToothContainer>(result);
    }

    setTeethToView();

    makeEdited();
}

void TreatmentPlanPresenter::addStage()
{
    if(m_treatmentPlan.is_completed) return;

    if(m_treatmentPlan.stages.size() == 9) return;

    if(m_selection.first == m_treatmentPlan.stages.size()-1 &&
        m_treatmentPlan.lastStageIsConclusion){
        return;
    }

    auto &stages = m_treatmentPlan.stages;

    MultilineDialog d("");
    d.setWindowTitle("Описание на етапа");
    d.enableTemplateLoading(DbNotes::TemplateType::PlanStage);
    d.enableEditing();

    auto stageDescription = d.getResult();

    if(!stageDescription.has_value()) return;

    auto newStage = TreatmentPlan::Stage();

    newStage.notes = stageDescription.value();

    if (m_selection.first == -1) {
        stages.push_back(newStage);
    } else {
        stages.insert(stages.begin() + (m_selection.first + 1), newStage);
    }

    m_selection.first += 1;
    m_selection.second = -1;

    view->setTreatmentPlan(m_treatmentPlan);

    view->setSelection(m_selection);

    makeEdited();

}

void TreatmentPlanPresenter::removeStage()
{
    if(m_treatmentPlan.is_completed) return;

    auto stage = getCurrentStage();

    if(!stage) return;

    auto &stages = m_treatmentPlan.stages;

    if(stage->plannedProcedures.size() &&
    !ModalDialogBuilder::askDialog(
        "Сигурни ли сте, че искате да премахнете етапа заедно с всички негови процедури?", false
    )) return;

    if(stage == getConclusion()){
        m_treatmentPlan.lastStageIsConclusion = false;
    }

    stages.erase(stages.begin() + m_selection.first);

    m_selection.first = std::min(m_selection.first, static_cast<int>(stages.size()) - 1);
    m_selection.second = -1;

    //only conclusion is left
    if(m_treatmentPlan.stages.size() == 1 && m_treatmentPlan.lastStageIsConclusion){
        m_treatmentPlan.lastStageIsConclusion = false;
        m_treatmentPlan.stages.clear();
        m_selection = {-1, -1};
    }

    view->setTreatmentPlan(m_treatmentPlan);

    view->setSelection(m_selection);

    setTeethToView();

    makeEdited();
}

void TreatmentPlanPresenter::addProcedure(const std::vector<int>& teeth_idx)
{
    if(m_treatmentPlan.is_completed) return;

    if(m_treatmentPlan.stages.empty()){
        ModalDialogBuilder::showMessage("За да добавите процедура първо добавете поне един етап");
        return;
    }

    if(m_selection.first == -1) return;

    if(m_treatmentPlan.lastStageIsConclusion &&
        m_treatmentPlan.stages.size()-1 == m_selection.first
    ){
        return;
    }

    auto &stage = m_treatmentPlan.stages[m_selection.first].plannedProcedures;

    int insertPos = m_selection.second == -1 ?
        stage.size()
        :
        m_selection.second + 1;

    ProcedureDialogPresenter d(m_treatmentPlan.teeth.getSelectedTeethPtr(teeth_idx));

    auto procedures = d.openDialog();

    for(auto& p : procedures){
        auto planned = TreatmentPlan::PlannedProcedure(p);

        planned.priceRange = User::getPrice(p.code.code());

        auto multiplier = p.getPriceMultiplier();

        planned.priceRange.first *= multiplier;
        planned.priceRange.second *= multiplier;

        stage.insert(stage.begin() + insertPos, planned);

        ++insertPos;
    }

    m_selection.second = insertPos - 1;

    view->setTreatmentPlan(m_treatmentPlan);

    view->setSelection(m_selection);

    setTeethToView();

    makeEdited();
}

void TreatmentPlanPresenter::addConclusion()
{
    if(m_treatmentPlan.lastStageIsConclusion){

        m_selection.first = m_treatmentPlan.stages.size()-1;
        m_selection.second = -1;
        view->setSelection(m_selection);
        editPressed();
        return;
    }

    if(m_treatmentPlan.stages.empty()){
        ModalDialogBuilder::showMessage("За да добавите заключение първо добавете поне един етап");
        return;
    }

    auto &stages = m_treatmentPlan.stages;

    MultilineDialog d("");
    d.setWindowTitle("Описание на етапа");
    d.enableTemplateLoading(DbNotes::TemplateType::PlanConclusion);
    d.enableEditing();

    auto conclusion = d.getResult();

    if(!conclusion.has_value()) return;

    stages.emplace_back();
    stages.back().notes = conclusion.value();

    m_treatmentPlan.lastStageIsConclusion = true;

    view->setTreatmentPlan(m_treatmentPlan);
}

void TreatmentPlanPresenter::editPressed()
{
    if(m_treatmentPlan.is_completed) return;

    if(m_selection.second == -1) {
            stageEditRequested();
            return;
    }

    auto currentProcedure = getCurrentProcedure();

    if(!currentProcedure) return;

    PlannedProcedureDialog d(*currentProcedure);

    auto result = d.getResult();

    if(!result) return;

    *currentProcedure = result.value();

    view->setTreatmentPlan(m_treatmentPlan);
}

void TreatmentPlanPresenter::removePressed()
{
    if(m_treatmentPlan.is_completed) return;

    m_selection.second == -1 ?
        removeStage()
        :
        removeProcedure();
}

void TreatmentPlanPresenter::dateChanged(const Date &date)
{
    m_treatmentPlan.date = date;

    makeEdited();
}

void TreatmentPlanPresenter::removeProcedure()
{
    auto currentProcedure = getCurrentProcedure();

    if(!currentProcedure) return;

    auto &planned = getCurrentStage()->plannedProcedures;

    if(!ModalDialogBuilder::askDialog("Сигурни ли сте, че искате да премахнете избраната процедура?"), false) return;

    if(currentProcedure->rowid){
        m_deleted_procedures.push_back(currentProcedure->rowid);
    }

    planned.erase(planned.begin() + m_selection.second);

    m_selection.second = planned.empty() ?
        -1
        :
        std::min(m_selection.second, static_cast<int>(planned.size()) - 1);


    view->setTreatmentPlan(m_treatmentPlan);

    view->setSelection(m_selection);

    setTeethToView();

    makeEdited();
}

bool TreatmentPlanPresenter::invalidSelection()
{
    if(m_selection.first == -1) return true;

    if(m_selection.first >= m_treatmentPlan.stages.size()) return true;

    if(m_selection.second > -1 && m_selection.second >= m_treatmentPlan.stages[m_selection.first].plannedProcedures .size()) return true;

    return false;
}

void TreatmentPlanPresenter::selectionChanged(const std::pair<int, int>& stageProcedurePair)
{
    m_selection = stageProcedurePair;

    view->setSelection(m_selection); //sometimes view and presenter get out of sync
    setTeethToView();
}

void TreatmentPlanPresenter::priceEditRequested()
{
    if(m_treatmentPlan.is_completed) return;

    if(m_selection.second == -1) return;

    auto& priceRange = m_treatmentPlan
                        .stages[m_selection.first]
                        .plannedProcedures[m_selection.second]
                        .priceRange;

    PriceInputDialog d(priceRange);

    auto result = d.getResult();

    if(!result) return;

    priceRange = result.value();

    view->setTreatmentPlan(m_treatmentPlan);

    view->setSelection(m_selection);

    makeEdited();
}

void TreatmentPlanPresenter::stageEditRequested()
{
    if(m_treatmentPlan.is_completed) return;

    auto stage = getCurrentStage();

    if(!stage) return;

    auto templateType = stage == getConclusion() ?
                            DbNotes::TemplateType::PlanConclusion : DbNotes::TemplateType::PlanStage;

    MultilineDialog d(stage->notes);
    d.setWindowTitle("Описание на етапа");
    d.enableTemplateLoading(templateType);
    d.enableEditing();

    auto result = d.getResult();

    if(!result) return;

    stage->notes = result.value();

    view->setTreatmentPlan(m_treatmentPlan);

    view->setSelection(m_selection);

    makeEdited();

    return;

}

void TreatmentPlanPresenter::setCompleted(bool completed)
{
    m_treatmentPlan.is_completed = completed;
    makeEdited();
}
