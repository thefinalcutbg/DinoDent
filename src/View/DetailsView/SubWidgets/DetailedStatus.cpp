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

	layout = new QVBoxLayout(ui.container);

	obtWidget = new ObturationWidget();
	crownWidget = new CrownWidget();
	implantWidget = new ImplantView();
	dentistWidget = new DentistMadeWidget();
	pathologyWidget = new PathologyWidget();

	ui.imageLabel->setStyleSheet("border: 1px solid lightgray");

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

void DetailedStatus::clearData()
{
	obtWidget->setParent(nullptr);
	crownWidget->setParent(nullptr);
	implantWidget->setParent(nullptr);
	dentistWidget->setParent(nullptr);
	pathologyWidget->setParent(nullptr);
}

template<typename L, typename W, typename D>
inline void setAndShow(L& layout, W& widget, const D& data) {
	widget->setData(data); 
	layout->addWidget(widget);
}

void DetailedStatus::setData(const ImplantData& data){
	setAndShow(layout, implantWidget, data);
}

void DetailedStatus::setData(const DentistData& data){
	layout->addWidget(dentistWidget);
}

void DetailedStatus::setData(const CrownData& data, const DentistData& dentist){
	setAndShow(layout, crownWidget, data);
	layout->addWidget(dentistWidget);

}

void DetailedStatus::setData(const ObturationData& data){
	setAndShow(layout, obtWidget, data); 
}

void DetailedStatus::setData(const PathologyData& data){
	layout->addWidget(pathologyWidget);
}

ObturationData DetailedStatus::getObturationData(){ 
	return obtWidget->getData();
}

ImplantData DetailedStatus::getImplantData(){
	return implantWidget->getData();
}

DentistData DetailedStatus::getDentistData(){
	return DentistData();
}

CrownData DetailedStatus::getCrownData(){
	return crownWidget->getData();
}

PathologyData DetailedStatus::getPathologyData()
{
	return PathologyData();
}


DetailedStatus::~DetailedStatus()
{
}

