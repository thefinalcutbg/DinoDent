#include "ProcedureHistoryDialog.h"
#include "Model/Dental/HisSnapshot.h"
#include "View/Graphics/TeethViewScene.h"

ProcedureHistoryDialog::ProcedureHistoryDialog(ProcedureHistoryPresenter& p)
    : QDialog(Q_NULLPTR), presenter(p)
{
    ui.setupUi(this);

    ui.tabWidget->setCurrentIndex(1);

    setWindowTitle("Онлайн пациентско досие");

    ui.graphicsView->setDisabled(true);
    ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto initTable = [](QTableView* table, QAbstractTableModel* model) {

        table->setModel(model);

        table->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);

        table->hideColumn(0);
        table->hideColumn(6);
        table->hideColumn(7);

        table->setColumnWidth(1, 90);
        table->setColumnWidth(2, 200);
        table->setColumnWidth(3, 25);
        table->setColumnWidth(4, 200);
        table->setColumnWidth(5, 49);
        table->setColumnWidth(8, 200);


        table->verticalHeader()->setDefaultSectionSize(20);
        table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        table->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
    };

    initTable(ui.pisTable, &pis_model);
    initTable(ui.hisTable, &his_model);

    ui.tabWidget->setTabIcon(0, QIcon(":/icons/icon_nhif.png"));
    ui.tabWidget->setTabIcon(1, QIcon(":/icons/icon_his.png"));
    ui.tabWidget->setTabIcon(2, QIcon(":/icons/icon_his.png"));

    ui.frame->setStyleSheet("QFrame{background-color:white;}");

    teeth_scene = new TeethViewScene();

    ui.graphicsView->setScene(teeth_scene);

    ToothContainer empty_container;

    for (int i = 0; i < 32; i++)
    {
        teeth_scene->display(ToothPaintHint(empty_container[i]));
    }

    connect(ui.applyButton, &QPushButton::clicked, this, [&] { presenter.pisApplyClicked(); });
    connect(ui.refreshPis, &QPushButton::clicked, [&] { presenter.refreshPIS(); });
    connect(ui.refreshHis, &QPushButton::clicked, [&] { presenter.refreshHIS(); });
    connect(ui.refreshStatus, &QPushButton::clicked, [&] { presenter.refreshStatus(); });
    connect(ui.applyCurrentStatus, &QPushButton::clicked, [&]{ presenter.statusApplyClicked(); });
    connect(ui.tabWidget, &QTabWidget::currentChanged, [&](int idx) { presenter.tabFocused(idx); });
    connect(ui.statusSlider, &QSlider::valueChanged, [&](int value) { presenter.sliderPositionChanged(value); });
    connect(ui.nextButton, &QPushButton::clicked, [&] { ui.statusSlider->setValue(ui.statusSlider->value() + 1); });
    connect(ui.prevButton, &QPushButton::clicked, [&] { ui.statusSlider->setValue(ui.statusSlider->value() - 1); });
    presenter.setView(this);
    
}

void ProcedureHistoryDialog::setPis(const std::vector<Procedure>& p)
{
    pis_model.setProcedures(p);
    ui.refreshPis->setText("Опресни");
}

void ProcedureHistoryDialog::setHis(const std::vector<Procedure>& h)
{
    his_model.setProcedures(h);
    ui.refreshHis->setText("Опресни");
}

void ProcedureHistoryDialog::setSnapshot(const HisSnapshot& s)
{
    for (int i = 0; i < 32; i++)
    {
        teeth_scene->display(ToothPaintHint(s.teeth[i]));
    }

    teeth_scene->setProcedures(s.affected_teeth);

    ui.nextButton->setDisabled(false);
    ui.prevButton->setDisabled(false);
    ui.statusSlider->setDisabled(false);

    ui.dateLabel->setText(QString("<b>Дата:</b> ") + s.date.toBgStandard().c_str());
    ui.procedureLabel->setText(QString("<b>Манипулация:</b> ") + s.procedure_name.c_str());
    ui.diagnosisLabel->setText(QString("<b>Диагноза:</b> ") + s.procedure_diagnosis.c_str());
    
    switch (s.financing)
    {
        case FinancingSource::NHIF: ui.financingLabel->setText("<b>Финансиране:</b> НЗОК"); break;
        case FinancingSource::Patient: ui.financingLabel->setText("<b>Финансиране:</b> Пациент"); break;
        case FinancingSource::PHIF: ui.financingLabel->setText("<b>Финансиране:</b> ДЗОФ"); break;
        default: ui.financingLabel->clear();
    }

    if (s.procedure_note.size()) {
        ui.notesLabel->setText(QString("<b>Бележки:</b> ") + s.procedure_note.c_str());
    }
    else {
        ui.notesLabel->clear();
    }

    ui.refreshStatus->setText("Опресни");
}

void ProcedureHistoryDialog::focusTab(int idx)
{
    ui.tabWidget->setCurrentIndex(idx);
}


void ProcedureHistoryDialog::closeDialog()
{
    close();
}

void ProcedureHistoryDialog::setSliderIndex(int index)
{
    ui.statusSlider->setValue(index);
}

void ProcedureHistoryDialog::setSliderCount(int count)
{
    ui.statusSlider->setRange(0, count);
}

ProcedureHistoryDialog::~ProcedureHistoryDialog()
{
}