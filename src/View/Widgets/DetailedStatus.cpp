#include "DetailedStatus.h"

//#include <QDebug>
#include <QIcon>
#include <QPainter>

#include "Presenter/CheckState.h"
#include "View/Graphics/ToothPainter.h"
#include "Presenter/DetailedStatusPresenter.h"
#include "Model/Dental/Enums.h"
/*
void DetailedStatus::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	painter.end();
}
*/
DetailedStatus::DetailedStatus(DetailedStatusPresenter* presenter) : presenter(presenter)
{

	enum TreeWidgetEnumType{ general, obturation, caries, mobility };

	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowFlags(Qt::Window);
	setWindowTitle("Детайли");


	ui.imageLabel->setStyleSheet("border: 1px solid lightgray");


	for (int i = 0; i < statusCount; i++)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setData(0, Qt::UserRole, general);
		item->setText(0, statusNames[i].data());
		item->setCheckState(0, Qt::Unchecked);
		ui.treeWidget->addTopLevelItem(item);
		
	}


	for(auto& name : surfName)
	{
		QTreeWidgetItem* obtSurf{ new QTreeWidgetItem() }, * carSurf{ new QTreeWidgetItem() };

		obtSurf->setText(0, name);
		carSurf->setText(0, name);
		carSurf->setData(0, Qt::UserRole, caries);
		obtSurf->setData(0, Qt::UserRole, obturation);
		obtSurf->setCheckState(0, Qt::Unchecked);
		carSurf->setCheckState(0, Qt::Unchecked);

		ui.treeWidget->topLevelItem(StatusCode::Obturation)->addChild(obtSurf);
		ui.treeWidget->topLevelItem(StatusCode::Caries)->addChild(carSurf);
	}

	for (auto& name : mobilityNames)
	{
		QTreeWidgetItem* degree{ new QTreeWidgetItem() };
		degree->setText(0, name.data());
		degree->setData(0, Qt::UserRole, mobility);
		degree->setCheckState(0, Qt::Unchecked);
		ui.treeWidget->topLevelItem(StatusCode::Mobility)->addChild(degree);

	}
	
	connect(ui.treeWidget, &QTreeWidget::itemChanged, this, [=](QTreeWidgetItem* item, int column) 
		{ 
			auto list = ui.treeWidget->selectedItems();

			if (list.isEmpty())
			{
				ui.treeWidget->setCurrentItem(item);
			}
			else if (list[0] != item) //selecting the item, which checkbox is checked;
			{
				ui.treeWidget->blockSignals(true);
				list[0]->setSelected(false);
				ui.treeWidget->blockSignals(false);

				ui.treeWidget->setCurrentItem(item);
			}

			bool checked = ui.treeWidget->currentItem()->checkState(0) == Qt::Checked ? true : false;

			presenter->checkStateChanged(checked);
			
		});

		connect(ui.treeWidget, &QTreeWidget::currentItemChanged, this, [=](QTreeWidgetItem* item) {
		
		int parent = ui.treeWidget->selectionModel()->currentIndex().parent().row();
		int code = ui.treeWidget->selectionModel()->currentIndex().row();
		
		if (presenter)
		{
			
			switch (item->data(0, Qt::UserRole).toInt())
			{
				case general: presenter->statusSelected(0, code); break;
				case obturation: presenter->statusSelected(1, code); break;
				case caries: presenter->statusSelected(2, code); break;
				case mobility: presenter->statusSelected(3, code); break;
			}
		}

		
		});


	connect(ui.okButton, &QPushButton::clicked, this, [=] {presenter->okPressed(); close(); });
	connect(ui.cancelButton, &QPushButton::clicked, this, [=] { close(); });
	
	presenter->setView(this);

	ui.notesEdit->setFocus();

}


void DetailedStatus::setCheckModel(const CheckModel& checkModel)
{
	QSignalBlocker blocker(ui.treeWidget);

	for (int i = 0; i < checkModel.generalStatus.size(); i++)
	{
		checkModel.generalStatus[i] == CheckState::checked ?
			ui.treeWidget->topLevelItem(i)->setCheckState(0, Qt::CheckState::Checked)
			:
			ui.treeWidget->topLevelItem(i)->setCheckState(0, Qt::CheckState::Unchecked);
	}

	for (int i = 0; i < checkModel.obturationStatus.size(); i++)
	{
		checkModel.obturationStatus[i] == CheckState::checked ?
			ui.treeWidget->topLevelItem(StatusCode::Obturation)->child(i)->setCheckState(0, Qt::CheckState::Checked)
			:
			ui.treeWidget->topLevelItem(StatusCode::Obturation)->child(i)->setCheckState(0, Qt::CheckState::Unchecked);
	}

	for (int i = 0; i < checkModel.cariesStatus.size(); i++)
	{
		checkModel.cariesStatus[i] == CheckState::checked ?
			ui.treeWidget->topLevelItem(StatusCode::Caries)->child(i)->setCheckState(0, Qt::CheckState::Checked)
			:
			ui.treeWidget->topLevelItem(StatusCode::Caries)->child(i)->setCheckState(0, Qt::CheckState::Unchecked);
	}

	for (int i = 0; i < checkModel.mobilityStatus.size(); i++)
	{
		checkModel.mobilityStatus[i] == CheckState::checked ?
			ui.treeWidget->topLevelItem(StatusCode::Mobility)->child(i)->setCheckState(0, Qt::CheckState::Checked)
			:
			ui.treeWidget->topLevelItem(StatusCode::Mobility)->child(i)->setCheckState(0, Qt::CheckState::Unchecked);
	}
}

void DetailedStatus::disableItem(int index, bool disabled)
{
	QSignalBlocker b(ui.treeWidget);

	auto item = ui.treeWidget->topLevelItem(index);

	if (disabled)
		item->setFlags(Qt::NoItemFlags);
	else
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
	
}

void DetailedStatus::paintTooth(const ToothPaintHint& hint)
{
	auto tooth = ToothPainter::getBuccalOcclusal(hint);
	ui.imageLabel->setPixmap(tooth);
}


void DetailedStatus::setData(const std::string& notesData)
{
	ui.notesEdit->setText(QString::fromStdString(notesData));
}


std::string DetailedStatus::getNotes(){ return ui.notesEdit->toPlainText().toStdString(); }


void DetailedStatus::setHistoryData(const std::vector<Procedure>& history)
{
	m_historyModel.setProcedures(history);
	ui.tableView->setModel(&m_historyModel);

	ui.tableView->horizontalHeader()->setHighlightSections(false);

	ui.tableView->verticalHeader()->setVisible(false);

	ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
	ui.tableView->verticalHeader()->setDefaultSectionSize(50);
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

	ui.tableView->hideColumn(0);
	ui.tableView->setColumnWidth(1, 69);
	ui.tableView->setColumnWidth(2, 150);
	ui.tableView->setColumnWidth(3, 25);
	ui.tableView->setColumnWidth(4, 150);
	ui.tableView->setColumnWidth(5, 49);
	ui.tableView->setColumnWidth(6, 70);
	ui.tableView->setColumnWidth(7, 150);
	ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
	ui.tableView->setShowGrid(false);
	
}

DetailedStatus::~DetailedStatus()
{

}
