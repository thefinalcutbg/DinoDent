#pragma once
#include <QDialog>

#include "Model/Dental/TreatmentPlan.h"

namespace Ui {
class PlannedProcedureDialog;
}

class PlannedProcedureDialog : public QDialog
{
    Q_OBJECT

    TreatmentPlan::PlannedProcedure m_procedure;

public:
    explicit PlannedProcedureDialog(TreatmentPlan::PlannedProcedure& p, QWidget *parent = nullptr);
    std::optional<TreatmentPlan::PlannedProcedure> getResult();
    ~PlannedProcedureDialog();

private:
    Ui::PlannedProcedureDialog *ui;
};
