#pragma once

#include <QWidget>

class TreatmentPlanPresenter;
class PatientTileInfo;
struct ToothPaintHint;
class TeethViewScene;
struct TreatmentPlan;

namespace Ui {
class TreatmentPlanView;
}

class TreatmentPlanView : public QWidget
{
    Q_OBJECT

    TreatmentPlanPresenter* presenter = nullptr;
    TeethViewScene* teethViewScene;

    void paintEvent(QPaintEvent* event) override;
    std::pair<int,int> getSelection() const;
    void disableEditFileds(bool disabled);

public:
    explicit TreatmentPlanView(QWidget *parent = nullptr);
    void setPresenter(TreatmentPlanPresenter* p);
    void repaintTooth(const ToothPaintHint& tooth);
    void setTreatmentPlan(const TreatmentPlan& p);
    void setAffectedTeeth(const std::vector<int>& indexes);
    void setSelection(const std::pair<int, int>& stageProcedurePair);
    PatientTileInfo* tileInfo();
    ~TreatmentPlanView();

private:
    Ui::TreatmentPlanView *ui;
};
