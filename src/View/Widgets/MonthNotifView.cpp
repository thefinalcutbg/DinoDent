﻿#include "MonthNotifView.h"
#include <QPainter>
#include <QDialog>

#include "Presenter/MonthNotifPresenter.h"

MonthNotifView::MonthNotifView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    ui.loadXMLbutton->setIcon(QIcon(":/icons/icon_xml.png"));
    ui.loadPISbutton->setIcon(QIcon(":/icons/icon_nhif.png"));

    ui.tableView->setModel(&model);

    ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    ui.tableView->horizontalHeader()->setHighlightSections(false);

    ui.tableView->verticalHeader()->hide();

    ui.tableView->setColumnWidth(0, 120);
    ui.tableView->setColumnWidth(1, 200);
    ui.tableView->setColumnWidth(2, 100);
    ui.tableView->setColumnWidth(3, 100);
    ui.tableView->hideColumn(4);

    ui.tableView->verticalHeader()->setDefaultSectionSize(20);
    ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
    ui.tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);


    connect(ui.okButton, &QPushButton::clicked, this, [=, this] {

        m_idx = ui.tableView->selectionModel()->currentIndex().row();

            presenter->okPressed(m_idx);
        });

    connect(ui.loadPISbutton, &QPushButton::clicked, this, [=, this] {presenter->loadFromPis();});
    connect(ui.loadXMLbutton, &QPushButton::clicked, this, [=, this] {presenter->loadFromXml();});
    connect(ui.tableView, &QTableView::doubleClicked, this, [=, this] { ui.okButton->click();});

}

void MonthNotifView::setPresenter(MonthNotifPresenter* presenter)
{
	this->presenter = presenter;
    presenter->setView(this);
}

void MonthNotifView::hasBeenShown()
{
	if (ui.tableView->model()->rowCount() == 0 && first_show) {
		ui.loadPISbutton->click();
		first_show = false;
	}
}

MonthNotifView::~MonthNotifView()
{}

void MonthNotifView::setMonthNotifRows(const std::vector<MonthNotifRow> rows)
{
    model.setRows(rows);

    if (rows.size()) {
        ui.tableView->selectRow(0);
        ui.loadPISbutton->setText("Опресни");
    }
}

void MonthNotifView::closeParentDialog()
{
    if (parent_dialog) { parent_dialog->close();}
}
