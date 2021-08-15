#include "DetailedStatus.h"
#include "Model/Tooth/Enums.h"
#include <QDebug>
#include "Presenter/ListPresenter/StatusPresenter/CheckState.h"
#include "View/ListView/ToothPaintDevices/ToothPainter.h"
#include "DetailsWidgets.h"
DetailedStatus::DetailedStatus(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.imageLabel->setStyleSheet("border: 1px solid lightgray");
	
	PathologyWidget* p = new PathologyWidget();
	ui.stackedWidget->addWidget(p);
	
	ImplantWidget* i = new ImplantWidget();
	ui.stackedWidget->addWidget(i);

	ObturationWidget* o = new ObturationWidget();
	ui.stackedWidget->addWidget(o);

	DentistMadeWidget* d = new DentistMadeWidget();
	ui.stackedWidget->addWidget(d);

	CrownWidget* c = new CrownWidget();
	ui.stackedWidget->addWidget(c);

	ui.stackedWidget->setCurrentWidget(i);



	for (auto& name : statusNames)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0, name);
		item->setCheckState(0, Qt::Unchecked);
		ui.treeWidget->addTopLevelItem(item);
		
	}

	for(auto& name : surfName)
	{
		QTreeWidgetItem* obtSurf{ new QTreeWidgetItem() }, * carSurf{ new QTreeWidgetItem() };
		obtSurf->setText(0, name);

		carSurf->setText(0, name);
		obtSurf->setCheckState(0, Qt::Unchecked);
		carSurf->setCheckState(0, Qt::Unchecked);
		ui.treeWidget->topLevelItem(StatusCode::Obturation)->addChild(obtSurf);
		ui.treeWidget->topLevelItem(StatusCode::Caries)->addChild(carSurf);
	}
	
	connect(ui.treeWidget, &QTreeWidget::itemChanged, [&](QTreeWidgetItem* item, int column) 
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

			emit itemChecked(checked);
			
		});
		
	connect(ui.treeWidget, &QTreeWidget::currentItemChanged, [&]() {

		int parent = ui.treeWidget->selectionModel()->currentIndex().parent().row();
		int code = ui.treeWidget->selectionModel()->currentIndex().row();

		if (parent == -1) ui.statusTitle->setText(statusNames[code]);
		else ui.statusTitle->setText(statusNames[parent] + " (" + surfName[code] + ")");

		emit selectionChanged(parent,code); 
		});

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
			ui.treeWidget->topLevelItem(1)->child(i)->setCheckState(0, Qt::CheckState::Checked)
			:
			ui.treeWidget->topLevelItem(1)->child(i)->setCheckState(0, Qt::CheckState::Unchecked);
	}

	for (int i = 0; i < checkModel.cariesStatus.size(); i++)
	{
		checkModel.cariesStatus[i] == CheckState::checked ?
			ui.treeWidget->topLevelItem(2)->child(i)->setCheckState(0, Qt::CheckState::Checked)
			:
			ui.treeWidget->topLevelItem(2)->child(i)->setCheckState(0, Qt::CheckState::Unchecked);
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
	auto original = painter.getPixmap(hint);
	ui.imageLabel->setPixmap(original.scaled(ui.imageLabel->width(), ui.imageLabel->height(), Qt::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation));
}


DetailedStatus::~DetailedStatus()
{
}

