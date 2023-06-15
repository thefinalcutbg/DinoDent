#include "ProcedureHistoryDialog.h"

#include "Presenter/ToothHintCreator.h"
#include "View/Graphics/TeethViewScene.h"

ProcedureHistoryDialog::ProcedureHistoryDialog(ProcedureHistoryPresenter& p)
    : QDialog(Q_NULLPTR), presenter(p)
{
    ui.setupUi(this);

    setWindowTitle("Онлайн пациентско досие");

    ui.graphicsView->setDisabled(true);

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

    ui.frame->setStyleSheet("background-color:white;");

    connect(ui.applyButton, &QPushButton::clicked, this, [&] { presenter.pisApplyClicked(); });
    connect(ui.refreshPis, &QPushButton::clicked, [&] { presenter.refreshPIS(); });
    connect(ui.refreshHis, &QPushButton::clicked, [&] { presenter.refreshHIS(); });
    connect(ui.refreshStatus, &QPushButton::clicked, [&] { presenter.refreshStatus(); });
    connect(ui.applyCurrentStatus, &QPushButton::clicked, [&]{ presenter.statusApplyClicked(); });
    connect(ui.tabWidget, &QTabWidget::currentChanged, [&](int idx) { presenter.tabFocused(idx); });
    presenter.setView(this);
}

ProcedureHistoryDialog::~ProcedureHistoryDialog()
{
}

void ProcedureHistoryDialog::setPis(const std::vector<Procedure>& p)
{
    pis_model.setProcedures(p);
}

void ProcedureHistoryDialog::setHis(const std::vector<Procedure>& h)
{
    his_model.setProcedures(h);
}

void ProcedureHistoryDialog::setCurrentStatus(const ToothContainer& teeth)
{
    TeethViewScene* s = new TeethViewScene();

    auto hints = ToothHintCreator::getTeethHint(teeth);

    ui.graphicsView->setScene(s);

    for (auto& hint : hints)
    {
        s->display(hint);
    }

}

void ProcedureHistoryDialog::focusTab(int idx)
{
    ui.tabWidget->setCurrentIndex(idx);
}


void ProcedureHistoryDialog::closeDialog()
{
    close();
}
