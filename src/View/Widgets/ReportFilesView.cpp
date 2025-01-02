#include "ReportFilesView.h"

ReportFilesView::ReportFilesView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.loadPISbutton->setIcon(QIcon(":/icons/icon_nhif.png"));

	ui.tableView->setModel(&model);

	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	ui.tableView->horizontalHeader()->setHighlightSections(false);

	ui.tableView->verticalHeader()->hide();

    ui.tableView->setColumnWidth(0, 120);
	ui.tableView->setColumnWidth(1, 190);
	ui.tableView->setColumnWidth(2, 200);
	ui.tableView->setColumnWidth(3, 120);
	ui.tableView->setColumnWidth(4, 100);

	ui.tableView->verticalHeader()->setDefaultSectionSize(20);
	ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	ui.tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    connect(ui.loadPISbutton, &QPushButton::clicked, this, [&] {presenter.refreshFiles();});

    connect(ui.tableView, &QTableView::clicked, this, [&] {

            if (ui.tableView->selectionModel() == nullptr) return;

			presenter.showError(ui.tableView->selectionModel()->currentIndex().row());
		
		});

	presenter.setView(this);
}

void ReportFilesView::setFiles(const std::vector<ReportFile>&fileRows)
{
	model.setRows(fileRows);
	
	if (fileRows.size()) {
		ui.loadPISbutton->setText("Опресни");
	}
}

ReportFilesView::~ReportFilesView()
{}
