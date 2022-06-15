#include "MonthNotifDialog.h"

MonthNotifDialog::MonthNotifDialog(const std::vector<MonthNotifRow>& rows, QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

    ui.tableView->setModel(&model);
    model.setRows(rows);

    ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    ui.tableView->horizontalHeader()->setHighlightSections(false);

    ui.tableView->setColumnWidth(0, 80);
    ui.tableView->setColumnWidth(1, 200);
    ui.tableView->setColumnWidth(2, 100);
    ui.tableView->setColumnWidth(3, 100);
    ui.tableView->setColumnWidth(4, 70);

    ui.tableView->verticalHeader()->setDefaultSectionSize(20);
    ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
    ui.tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

  
    connect(ui.okButton, &QPushButton::clicked, [=] {

                m_idx = ui.tableView->selectionModel()->currentIndex().row();

                if (m_idx == -1) {
                    return;
                }

                close();
        });

    connect(ui.cancelButton, &QPushButton::clicked, [=] { m_idx = -1; close();});
    connect(ui.tableView, &QTableView::doubleClicked, [=] { ui.okButton->click();});
}

int MonthNotifDialog::exec()
{
    QDialog::exec();
    return m_idx;
}

MonthNotifDialog::~MonthNotifDialog()
{
}
