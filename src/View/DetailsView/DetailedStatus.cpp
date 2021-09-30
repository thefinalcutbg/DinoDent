#include "DetailedStatus.h"

#include <QDebug>
#include <QIcon>
#include <QPainter>

#include "Presenter/ListPresenter/CheckState.h"
#include "View/ToothPaintDevices/ToothPainter.h"
#include "Presenter/DetailsPresenter/DetailedStatusPresenter.h"
#include "Model/Tooth/Enums.h"

void DetailedStatus::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	painter.end();
}

DetailedStatus::DetailedStatus(DetailedStatusPresenter* presenter) : presenter(presenter)
{
	static QIcon notesIcon(QPixmap("notes.png"));

	enum TreeWidgetEnumType{ general, obturation, caries, notes };

	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowFlags(Qt::Window);
	setWindowTitle("Tooth Details");

	layout = new QVBoxLayout(ui.container);
	layout->setContentsMargins(9, 9, 9, 0);
	layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding));
	obtWidget = new ObturationWidget();
	crownWidget = new CrownWidget();
	implantWidget = new ImplantView();
	dentistWidget = new DentistMadeWidget();
	pathologyWidget = new PathologyWidget();
	notesWidget = new QTextEdit();

	ui.imageLabel->setStyleSheet("border: 1px solid lightgray");

	QTreeWidgetItem* notesItem = new QTreeWidgetItem();
	notesItem->setText(0, u8"Бележки");
	notesItem->setData(0, Qt::UserRole, notes);
	notesItem->setIcon(0, notesIcon);
	ui.treeWidget->addTopLevelItem(notesItem);


	for (auto& name : statusNames)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setData(0, Qt::UserRole, general);
		item->setText(0, name);
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

		ui.treeWidget->topLevelItem(StatusCode::Obturation+1)->addChild(obtSurf);
		ui.treeWidget->topLevelItem(StatusCode::Caries+1)->addChild(carSurf);
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

			presenter->checkStateChanged(checked);
			
		});
		
	connect(ui.treeWidget, &QTreeWidget::currentItemChanged, [&](QTreeWidgetItem* item) {
		
		int parent = ui.treeWidget->selectionModel()->currentIndex().parent().row();
		int code = ui.treeWidget->selectionModel()->currentIndex().row();
		
		switch (item->data(0, Qt::UserRole).toInt())
		{
			case general: presenter->statusSelected(0, code - 1); break;
			case obturation: presenter->statusSelected(1, code); break;
			case caries: presenter->statusSelected(2, code); break;
			case notes: presenter->statusSelected(3, code); break;
		}
		qDebug() << "item changed" << item->text(0) << parent << code;
		if (parent == -1) ui.statusTitle->setText(item->text(0));
		else ui.statusTitle->setText(item->parent()->text(0) + " (" + item->text(0) + ")");

		
		});

	connect(dentistWidget, &DentistMadeWidget::checked, [&] { presenter->stateChanged(); });



	connect(ui.okButton, &QPushButton::clicked, [&] {presenter->okPressed(); close(); });
	connect(ui.cancelButton, &QPushButton::clicked, [&] { close(); });

	presenter->setView(this);

	//notesItem->setSelected(true);

}


void DetailedStatus::setCheckModel(const CheckModel& checkModel)
{
	QSignalBlocker blocker(ui.treeWidget);

	for (int i = 0; i < checkModel.generalStatus.size(); i++)
	{
		checkModel.generalStatus[i] == CheckState::checked ?
			ui.treeWidget->topLevelItem(i+1)->setCheckState(0, Qt::CheckState::Checked)
			:
			ui.treeWidget->topLevelItem(i+1)->setCheckState(0, Qt::CheckState::Unchecked);
	}

	for (int i = 0; i < checkModel.obturationStatus.size(); i++)
	{
		checkModel.obturationStatus[i] == CheckState::checked ?
			ui.treeWidget->topLevelItem(2)->child(i)->setCheckState(0, Qt::CheckState::Checked)
			:
			ui.treeWidget->topLevelItem(2)->child(i)->setCheckState(0, Qt::CheckState::Unchecked);
	}

	for (int i = 0; i < checkModel.cariesStatus.size(); i++)
	{
		checkModel.cariesStatus[i] == CheckState::checked ?
			ui.treeWidget->topLevelItem(3)->child(i)->setCheckState(0, Qt::CheckState::Checked)
			:
			ui.treeWidget->topLevelItem(3)->child(i)->setCheckState(0, Qt::CheckState::Unchecked);
	}
}

void DetailedStatus::disableItem(int index, bool disabled)
{
	QSignalBlocker b(ui.treeWidget);

	auto item = ui.treeWidget->topLevelItem(index+1);

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

void DetailedStatus::clearData()
{
	ui.statusTitle->setText("");
	obtWidget->setParent(nullptr);
	crownWidget->setParent(nullptr);
	implantWidget->setParent(nullptr);
	dentistWidget->setParent(nullptr);
	pathologyWidget->setParent(nullptr);
	notesWidget->setParent(nullptr);
}

void DetailedStatus::disableDetails(bool disabled)
{
	ui.container->setDisabled(disabled);

	ui.statusTitle->setStyleSheet(disabled ? "color: lightgray" : "color : black");

}

template<typename L, typename W, typename D>
inline void setAndShow(L& layout, W& widget, const D& data) {
	widget->setData(data); 
	layout->insertWidget(0, widget);
}

void DetailedStatus::setData(const ImplantData& data){setAndShow(layout, implantWidget, data);}
void DetailedStatus::setData(const DentistData& data){setAndShow(layout, dentistWidget, data);}
void DetailedStatus::setData(const CrownData& data){setAndShow(layout, crownWidget, data);}
void DetailedStatus::setData(const ObturationData& data){setAndShow(layout, obtWidget, data);}
void DetailedStatus::setData(const PathologyData& data){setAndShow(layout, pathologyWidget, data);}

void DetailedStatus::setData(const std::string& notesData)
{
	notesWidget->setText(QString::fromStdString(notesData));
	layout->addWidget(notesWidget);

}

bool DetailedStatus::getDentistData() { return dentistWidget->userChecked(); }
int DetailedStatus::getPathologyData() { return pathologyWidget->getData(); }
ObturationData DetailedStatus::getObturationData(){  return obtWidget->getData();}
ImplantData DetailedStatus::getImplantData(){ return implantWidget->getData();}
CrownData DetailedStatus::getCrownData(){return crownWidget->getData();}
std::string DetailedStatus::getNotes()
{
	return notesWidget->toPlainText().toStdString();
}

void DetailedStatus::setHistoryData(const std::vector<Procedure>& history)
{
	m_historyModel.setProcedures(history);
	ui.tableView->setModel(&m_historyModel);
	ui.tableView->setProcedureHistoryLayout();
}

DetailedStatus::~DetailedStatus()
{
	clearData();

	delete obtWidget;
	delete crownWidget;
	delete implantWidget;;
	delete dentistWidget;
	delete pathologyWidget;
	delete notesWidget;
}

