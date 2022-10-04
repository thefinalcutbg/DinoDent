#include "ReportFilesView.h"

#include <QPainter>

#include "View/ModalDialogBuilder.h"

ReportFilesView::ReportFilesView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.tableView->setModel(&model);

	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	ui.tableView->horizontalHeader()->setHighlightSections(false);

	ui.tableView->verticalHeader()->hide();

	ui.tableView->setColumnWidth(0, 150);
	ui.tableView->setColumnWidth(1, 150);
	ui.tableView->setColumnWidth(2, 150);
	ui.tableView->setColumnWidth(3, 100);
	ui.tableView->setColumnWidth(4, 100);

	ui.tableView->verticalHeader()->setDefaultSectionSize(20);
	ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	//ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
	ui.tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

	connect(ui.loadPISbutton, &QPushButton::clicked, [&] {presenter.refreshFiles();});
	connect(ui.tableView, &QTableView::doubleClicked, 
		
		[=] { 
		
			if (ui.tableView->selectionModel() == nullptr) return;

			presenter.showError(ui.tableView->selectionModel()->currentIndex().row());
		
		});

	presenter.setView(this);
}

void ReportFilesView::setFiles(const std::vector<ReportFile>&fileRows)
{
	model.setRows(fileRows);
	
	if (fileRows.size()) {
		ui.loadPISbutton->setText(u8"Опресни");
	}
}

void ReportFilesView::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}

ReportFilesView::~ReportFilesView()
{}
