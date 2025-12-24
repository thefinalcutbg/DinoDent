#include "TreatmentPlanPresenter.h"

#include "View/Widgets/TabView.h"
#include "View/Widgets/TreatmentPlanView.h"
#include "View/Widgets/PriceInputDialog.h"
#include "Model/User.h"
#include "ProcedureDialogPresenter.h"
#include "View/Widgets/MultilineDialog.h"
#include "Database/DbTreatmentPlan.h"

TreatmentPlanPresenter::TreatmentPlanPresenter(TabView* tabView, std::shared_ptr<Patient> patient, long long rowId)
    :
    TabInstance(tabView, TabType::TreatmentPlan, patient),
    patient_info(tabView->treatmentPlanView()->tileInfo(), patient),
    view(tabView->treatmentPlanView())
{
    m_treatmentPlan = DbTreatmentPlan::get(rowId);

    if(m_treatmentPlan.rowid){

        m_selection.first = m_treatmentPlan.stages.size()-1;

        if(m_selection.first > -1){
            m_selection.second = m_treatmentPlan.stages[m_selection.first].plannedProcedures.size()-1;
        }

        return;
    }

    //if it is a new plan:
    m_treatmentPlan.teeth = DbAmbList::getStatus(patient->rowid, m_treatmentPlan.date);
}

void TreatmentPlanPresenter::setDataToView()
{
    view->setPresenter(this);

    patient_info.setCurrent(true);

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
                       DbTreatmentPlan::update(m_treatmentPlan);

    if(!success) return false;

    edited = false;

    refreshTabName();

    return true;

}

void TreatmentPlanPresenter::print()
{

}

void TreatmentPlanPresenter::pdfPrint()
{

}

long long TreatmentPlanPresenter::rowID() const
{
    return m_treatmentPlan.rowid;
}

void TreatmentPlanPresenter::addStage()
{   
    if(m_treatmentPlan.stages.size() == 9) return;

    auto &stages = m_treatmentPlan.stages;

    MultilineDialog d("");
    d.setWindowTitle("Описание на етапа");
    d.enableEditing(false);

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
    auto &stages = m_treatmentPlan.stages;

    if (m_selection.first == -1) return;

    if(m_treatmentPlan.stages[m_selection.first].plannedProcedures.size() &&
    !ModalDialogBuilder::askDialog(
        "Сигурни ли сте, че искате да премахнете етапа заедно с всички негови процедури?", false
    )) return;

    stages.erase(stages.begin() + m_selection.first);

    view->setTreatmentPlan(m_treatmentPlan);

    m_selection.first = std::min(m_selection.first, static_cast<int>(stages.size()) - 1);
    m_selection.second = -1;

    view->setTreatmentPlan(m_treatmentPlan);

    view->setSelection(m_selection);

    setTeethToView();

    makeEdited();
}

void TreatmentPlanPresenter::addProcedure(const std::vector<int>& teeth_idx)
{
    if(m_selection.first == -1) return;

    if(m_treatmentPlan.stages.empty()){
        ModalDialogBuilder::showMessage("За да добавите процедура, първо добавете поне един етап");
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

void TreatmentPlanPresenter::editPressed()
{
    m_selection.second == -1 ?
        nameEditRequested() : priceEditRequested();
}

void TreatmentPlanPresenter::removePressed()
{
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
    if(m_selection.second == -1) return;

    if (m_selection.first == -1) return;

    if(!ModalDialogBuilder::askDialog(
            "Сигурни ли сте, че искате да премахнете избраната процедура?", false
    )) return;

    auto &planned = m_treatmentPlan.stages[m_selection.first].plannedProcedures;

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

    setTeethToView();
}

void TreatmentPlanPresenter::priceEditRequested()
{

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
}

void TreatmentPlanPresenter::nameEditRequested()
{

    if(m_selection.first == -1) return;

    if(m_selection.second == -1){

        auto& textForEdit = m_treatmentPlan.stages[m_selection.first].notes;

        MultilineDialog d(textForEdit);
        d.setWindowTitle("Описание на етапа");
        d.enableEditing(false);

        auto result = d.getResult();

        if(!result) return;

        textForEdit = result.value();

        view->setTreatmentPlan(m_treatmentPlan);

        view->setSelection(m_selection);

        makeEdited();

        return;
    }

    auto& procedure =
        m_treatmentPlan
        .stages[m_selection.first]
        .plannedProcedures[m_selection.second];

    auto text = procedure.name.empty() ? procedure.code.name() : procedure.name;

    auto result = ModalDialogBuilder::getStringInput("", "Редактиране на процедурата", text);

    if(!result) return;

    if(result->empty()){
        procedure.name.clear();
    } else {
        procedure.name = result.value();
    }

    view->setTreatmentPlan(m_treatmentPlan);

    view->setSelection(m_selection);

    makeEdited();
}
