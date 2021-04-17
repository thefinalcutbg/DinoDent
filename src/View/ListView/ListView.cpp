#include "ListView.h"

ListView::ListView(Database* database, QWidget* parent)
	: QWidget(parent), presenter(this), allergiesDialog(this)
{
	ui.setupUi(this);

	contextMenu = new ContextMenu(&presenter);
	ui.controlPanel->setStatusControl(&presenter);
	ui.surfacePanel->getPresenter()->getStatusControl(&presenter);

	teethViewScene = new TeethViewScene(&presenter, ui.teethView);
	teethViewScene->setContextMenu(contextMenu);

	ui.teethView->setDragMode(QGraphicsView::RubberBandDrag);
	ui.teethView->setScene(teethViewScene);

	presenter.setDialogPresnters(allergiesDialog.getPresenter());

	ui.procedureTable->setModel(&model);
	ui.procedureTable->setDimensions();

	connect(teethViewScene, &QGraphicsScene::selectionChanged, [=]{updateSelectedTeeth(); });
	connect(ui.statusEdit, &QLineEdit::textChanged, [=] {presenter.statusChanged(ui.statusEdit->text().toStdString());});
	connect(ui.patientTile, &QAbstractButton::clicked, [=] { presenter.openPatientDialog(); });
	connect(ui.allergiesTile, &QAbstractButton::clicked, [=] { presenter.openAllergiesDialog(); });
	connect(ui.procedureButton, &QAbstractButton::clicked, [=] { presenter.addProcedure(); });
	connect(ui.procedureTable, &ProcedureTable::deletePressed, [=] { ui.deleteProcedure->click(); });

	connect(ui.deleteProcedure, &QAbstractButton::clicked, 
		[=] {
			int currentIdx = ui.procedureTable->selectionModel()->currentIndex().row();
			int lastIdx = ui.procedureTable->verticalHeader()->count()-1;

			if (currentIdx == -1) return;

			presenter.deleteProcedure(currentIdx);

			if (currentIdx == lastIdx)
			{
				ui.procedureTable->selectRow(currentIdx - 1);
			}
			else ui.procedureTable->selectRow(currentIdx);
		});

	connect(ui.procedureTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, 
		[=] 
		{
			int row = ui.procedureTable->selectionModel()->currentIndex().row();

			if (row == -1) {

				presenter.manipulationSelected(row);
				return;
			}

			int manipulationIdx = ui.procedureTable->model()->index(row, 0).data().toInt();
			presenter.manipulationSelected(row);
		}
	);
}

void ListView::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(0, 0, width(), height(), Qt::white);

	painter.end();
}

void ListView::updateSelectedTeeth()
{
	std::vector<int> selectedIndexes;
	selectedIndexes.reserve(32);

	SelectionBox* selection;

	for (QGraphicsItem* item : teethViewScene->selectedItems())
	{
		selection = static_cast<SelectionBox*>(item);
		selectedIndexes.emplace_back(selection->getIndex());
	}
	
	std::sort(selectedIndexes.begin(), selectedIndexes.end());

	presenter.setSelectedTeeth(selectedIndexes);

}

ListPresenter* ListView::Presenter()
{
	return &this->presenter;
}

void ListView::refresh(AmbList& ambList, Patient& patient, std::vector<int>& selectedIndexes)
{
	//remember to block the signals!
	ui.patientTile->setPatient(patient);
	ui.allergiesTile->setPatient(patient);

	ui.statusEdit->blockSignals(1);
	ui.statusEdit->setText(QString::fromStdString(ambList.test));
	ui.statusEdit->blockSignals(0);

	for (int i = 0; i < 32; i++)
	{
		teethViewScene->display(ambList.teeth[i]);
	}

	teethViewScene->blockSignals(1);
	teethViewScene->setSelectedTeeth(selectedIndexes);
	teethViewScene->blockSignals(0);
}

void ListView::setCheckModel(const CheckModel& checkModel)
{
	ui.controlPanel->setModel(checkModel);
	contextMenu->setModel(checkModel);
}

void ListView::repaintTooth(const Tooth& tooth)
{
	teethViewScene->display(tooth);
	ui.teethView->setFocus();
}

void ListView::repaintBridges(const std::array<BridgeAppearance, 32> bridges)
{
	teethViewScene->display(bridges);
}

void ListView::updateControlPanel(const Tooth* tooth)
{
	ui.surfacePanel->getPresenter()->setTooth(tooth);
}

void ListView::setManipulations(const std::vector<RowData>& m)
{
	model.setManipulations(m);

	int tableHeight = m.size() * 50 + 26;
	//ne sym siguren izob6to, 4e taka iskam da izglejda:
	auto size = ui.procedureTable->size();
	size.setHeight(tableHeight);
	ui.procedureTable->setFixedSize(size);
	this->setFixedHeight(710 + tableHeight + 100);

	if (!m.size())
	{
		ui.procedureTable->hide();

	}
	else
	{
		ui.procedureTable->show();
	}
}

void ListView::openProcedureDialog(ProcedureDialogPresenter *p)
{
	ProcedureDialog dialog(p);
	dialog.openProcedureDialog();
}


ListView::~ListView()
{
	ui.teethView->blockSignals(true); //have to fix setting selected teeth to non-existing entity! (vector throws exception)

	delete teethViewScene;
	delete contextMenu;
}
