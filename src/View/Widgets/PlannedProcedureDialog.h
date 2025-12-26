#pragma once
#include <QDialog>

#include "Model/Dental/TreatmentPlan.h"

namespace Ui {
class PlannedProcedureDialog;
}

class PlannedProcedureDialog : public QDialog
{
    Q_OBJECT

    TreatmentPlan::PlannedProcedure& procedure_ref;

public:
    explicit PlannedProcedureDialog(TreatmentPlan::PlannedProcedure& p, QWidget *parent = nullptr);
    ~PlannedProcedureDialog();

private:
    Ui::PlannedProcedureDialog *ui;
};
