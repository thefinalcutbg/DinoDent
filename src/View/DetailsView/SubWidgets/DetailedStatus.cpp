#include "DetailedStatus.h"
#include "Model/Tooth/Enums.h"

#include "Presenter/ListPresenter/StatusPresenter/CheckState.h"

DetailedStatus::DetailedStatus(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QString statusNames[statusCount] =
	{ u8"Временен зъб", u8"Обтурация", u8"Кариес",  u8"Пулпит", u8"Периодонтит",
	  u8"Ендодонтско лечение", u8"Радикуларен щифт", u8"Корен",u8"Фрактура", u8"Екстракция",
	  u8"Пародонтит", u8"Подвижност I", u8"Подвижност II", u8"Подвижност III",
	  u8"Корона", u8"Мост/Блок корони",  u8"Имплант", u8"Свръхброен зъб" };

	QString surfName[surfaceCount] = 
	{ u8"Оклузално", u8"Медиално", u8"Дистално", 
		u8"Букално", u8"Лингвално", u8"Цервикално" };

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
			auto previouslySelected = ui.treeWidget->selectedItems().at(0);

			if ( previouslySelected != item) //selecting the item, which checkbox is checked;
			{
				ui.treeWidget->blockSignals(true);
				previouslySelected->setSelected(false);
				ui.treeWidget->blockSignals(false);

				ui.treeWidget->setCurrentItem(item);
			}

			emit itemChecked();
			
		});
		
	connect(ui.treeWidget, &QTreeWidget::currentItemChanged, [&]() {
		emit selectionChanged(ui.treeWidget->selectionModel()->currentIndex().parent().row(),
			ui.treeWidget->selectionModel()->currentIndex().row()); });

}


void DetailedStatus::setCheckModel(const CheckModel& checkModel)
{
	for (int i = 0; i < checkModel.generalStatus.size(); i++)
	{
		checkModel.generalStatus[i] == CheckState::checked ?
			ui.treeWidget->topLevelItem(i)->setCheckState(0, Qt::CheckState::Checked)
			:
			ui.treeWidget->topLevelItem(i)->setCheckState(0, Qt::CheckState::Unchecked);
	}

	for (int i = 0; i < checkModel.obturationStatus.size(); i++)
	{
		checkModel.generalStatus[i] == CheckState::checked ?
			ui.treeWidget->topLevelItem(1)->child(i)->setCheckState(0, Qt::CheckState::Checked)
			:
			ui.treeWidget->topLevelItem(1)->child(i)->setCheckState(0, Qt::CheckState::Unchecked);
	}

	for (int i = 0; i < checkModel.cariesStatus.size(); i++)
	{
		checkModel.generalStatus[i] == CheckState::checked ?
			ui.treeWidget->topLevelItem(2)->child(i)->setCheckState(0, Qt::CheckState::Checked)
			:
			ui.treeWidget->topLevelItem(2)->child(i)->setCheckState(0, Qt::CheckState::Unchecked);
	}
}


DetailedStatus::~DetailedStatus()
{
}

