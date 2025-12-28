#include "PlannedProcedureDialog.h"
#include "ui_PlannedProcedureDialog.h"

PlannedProcedureDialog::PlannedProcedureDialog(TreatmentPlan::PlannedProcedure& p,QWidget *parent)
    : QDialog(parent)
    , m_procedure(p),
    ui(new Ui::PlannedProcedureDialog)
{
    ui->setupUi(this);

    setWindowTitle("Редактиране на процедура");

    ui->nameEdit->setText(p.getNameText().c_str());

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

    ui->nameEdit->setFocus();
    ui->nameEdit->selectAll();

    connect(ui->okButton, &QPushButton::clicked, this, [=, this]{

        if(ui->procedureInput->isValid().size()) return;

        auto resultData = ui->procedureInput->getResult();

        m_procedure.diagnosis = resultData.diagnosis;

        m_procedure.notes = resultData.notes;

        auto text = ui->nameEdit->text().toStdString();

        if(text != m_procedure.code.name()){
            m_procedure.name = text;
        }

        m_procedure.priceRange = ui->priceInput->getPrice();

        accept();

    });
}

std::optional<TreatmentPlan::PlannedProcedure> PlannedProcedureDialog::getResult()
{
    if(exec() != QDialog::Accepted) return {};

    return m_procedure;
}

PlannedProcedureDialog::~PlannedProcedureDialog()
{
    delete ui;
}
