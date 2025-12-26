#include "PlannedProcedureDialog.h"
#include "ui_PlannedProcedureDialog.h"

PlannedProcedureDialog::PlannedProcedureDialog(TreatmentPlan::PlannedProcedure& p,QWidget *parent)
    : QDialog(parent)
    , procedure_ref(p),
    ui(new Ui::PlannedProcedureDialog)
{
    ui->setupUi(this);


    ui->nameEdit->setText(p.code.name().c_str());

    ui->priceInput->setPrice(p.priceRange);

    ui->procedureInput->disablePost();

    ui->procedureInput->setTreatmentPlanMode();

    ui->procedureInput->setCommonData(
        ProcedureInput::CommonData{
            .diagnosis = p.diagnosis,
            .notes = p.notes
        }, false
    );

    ui->procedureInput->setParameterData();

    connect(ui->okButton, &QPushButton::clicked, this, [=, this]{

        if(ui->procedureInput->isValid().size()) return;

        auto resultData = ui->procedureInput->getResult();

        procedure_ref.diagnosis = resultData.diagnosis;

        procedure_ref.notes = resultData.notes;

        auto text = ui->nameEdit->text().toStdString();

        if(text != procedure_ref.code.name()){
            procedure_ref.name = text;
        }

        procedure_ref.priceRange = ui->priceInput->getPrice();

        accept();

    });
}

PlannedProcedureDialog::~PlannedProcedureDialog()
{
    delete ui;
}
