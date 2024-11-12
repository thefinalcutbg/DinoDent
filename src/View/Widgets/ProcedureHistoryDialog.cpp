#include "ProcedureHistoryDialog.h"
#include "Model/Dental/HisSnapshot.h"
#include "View/Graphics/TeethViewScene.h"

ProcedureHistoryDialog::ProcedureHistoryDialog(ProcedureHistoryPresenter& p)
    : QDialog(Q_NULLPTR), presenter(p)
{
    ui.setupUi(this);

    ui.tabWidget->setCurrentIndex(1);

    setWindowTitle("Онлайн пациентско досие");

    auto initTable = [](QTableView* table, QAbstractTableModel* model) {

        table->setModel(model);

        table->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);

        table->hideColumn(0);
        table->hideColumn(6);
        table->hideColumn(7);

        table->setColumnWidth(1, 110);
        table->setColumnWidth(2, 200);
        table->setColumnWidth(3, 65);
        table->setColumnWidth(4, 200);
        table->setColumnWidth(5, 65);
        table->setColumnWidth(8, 200);

        table->verticalHeader()->setHidden(true);
        table->verticalHeader()->setDefaultSectionSize(20);
        table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        table->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
    };

    initTable(ui.pisTable, &pis_model);
    initTable(ui.hisTable, &his_model);


    ui.hospiTable->setModel(&hospi_model);
    ui.hospiTable->setColumnWidth(0, 120);
    ui.hospiTable->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
    ui.hospiTable->verticalHeader()->setDefaultSectionSize(20);
    ui.hospiTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.hospiTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.hospiTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
   
    ui.hisTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

    ui.tabWidget->setTabIcon(0, QIcon(":/icons/icon_nhif.png"));
    ui.tabWidget->setTabIcon(1, QIcon(":/icons/icon_his.png"));
    ui.tabWidget->setTabIcon(2, QIcon(":/icons/icon_his.png"));
    ui.tabWidget->setTabIcon(3, QIcon(":/icons/icon_hospital.png"));

  //  ui.frame->setStyleSheet("QFrame{background-color:white;}");

    connect(ui.applyButton, &QPushButton::clicked, this, [&] { presenter.pisApplyClicked(); });
    connect(ui.refreshPis, &QPushButton::clicked, this, [&] { presenter.refreshPIS(); });
    connect(ui.refreshHis, &QPushButton::clicked, this, [&] { presenter.refreshHIS(); });
    connect(ui.refreshStatus, &QPushButton::clicked, this, [&] { presenter.refreshStatus(); });
    connect(ui.refreshHospi, &QPushButton::clicked, this, [&] { presenter.refreshHospitalizations(); });
    connect(ui.applyCurrentStatus, &QPushButton::clicked, this, [&]{ presenter.statusApplyClicked(); });
    connect(ui.tabWidget, &QTabWidget::currentChanged, this, [&](int idx) { presenter.tabFocused(idx); });
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

void ProcedureHistoryDialog::setHospitalizations(const std::vector<Hospitalization>& h)
{
    hospi_model.setRows(h);
    ui.refreshHospi->setText("Опресни");
}

void ProcedureHistoryDialog::setSnapshots(const std::vector<HisSnapshot>& snapshots)
{
    ui.snapshotViewer->setSnapshots(snapshots);

    ui.refreshStatus->setText("Опресни");
}

SnapshotViewer* ProcedureHistoryDialog::getSnapshotViewer()
{
    return ui.snapshotViewer;
}


void ProcedureHistoryDialog::focusTab(int idx)
{
    ui.tabWidget->setCurrentIndex(idx);
}


void ProcedureHistoryDialog::closeDialog()
{
    close();
}

ProcedureHistoryDialog::~ProcedureHistoryDialog()
{
}
